#include "Application.h"
#include "WindowEventPublisher.h"
#include "ShaderLoader.h"
#include "PointcloudHandler.h"

#include "helpers.h"

int Application::AppMain() {
    basic_hole.radius = 2.0f;
    basic_hole.depth = 1.5;
    basic_hole.center = glm::vec3(0.0f, -2.4f, -18.0f);
    stbi_set_flip_vertically_on_load(true);

    GLuint quadVBO, quadVAO, EBO;
    glGenBuffers(1, &quadVBO);
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(quadVAO);

    float quad[] = {
            // positions          // texture coords
            1.0f,  1.0f, 0.0f,    1.0f, 1.0f, // top right
            1.0f, -1.0f, 0.0f,    1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    ShaderLoader ourShader("vertexShader.shader", "fragmentShader.shader");
    ourShader.setInt("texture0",0);
    imu_data = CarlaImuParser("../resources/imu.txt");
    std::vector<Pointcloud> pointcloud_list;

    auto files = glob("../resources/*.ply");

    textures = std::vector<GLuint>(files.size());
    glGenTextures(files.size(),  &textures[0]);

    for(size_t i = 0; i < files.size(); i++) {
        pointcloud_list.emplace_back(files[i]);

        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, nrChannels;
        unsigned char *data = stbi_load( (files[i].substr(0, files[i].size() - 4) + ".png").c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
    pclHand = PointcloudHandler(pointcloud_list);

    pclHand.pclList[0].model = glm::mat4(1.0f)*Carla_to_Opengl_coordinates;
    for(size_t i = 1 ; i < pclHand.pclList.size(); i++){
        glm::vec4 accel_carla = glm::vec4(imu_data.accel[i].x, imu_data.accel[i].y, 0.0f,1.0f);//(8.108274, 0.061310, 0.0, 1.0f);

        glm::vec3 accel_opengl = glm::transpose(imu_carla_to_opengl_coords) * accel_carla  ;

        float dt = imu_data.timestamp[i] - imu_data.timestamp[i-1];
        velocity += accel_opengl*dt;
        world_to_lidar = glm::translate(world_to_lidar, velocity + 0.5f*accel_opengl*(dt*dt));
        pclHand.pclList[i].model =  world_to_lidar*Carla_to_Opengl_coordinates;
    }
    WindowEventPublisher::addKeyboardListener(pclHand);

    ourShader.use();

    ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    while (!glfwWindowShouldClose(window))
    {
        camera.SetFollowingObject(&pclHand.pclList[pclHand.index]);


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        imGuiDrawWindow(basic_hole.radius, basic_hole.depth, clear_color);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();


        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);


        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[pclHand.index]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwPollEvents();
        WindowEventPublisher::notifyFrameUpdate(window, deltaTime);
        glfwSwapBuffers(window);
        continue;

        //ourShader.setMat4("model", world_to_lidar*Carla_to_Opengl_coordinates);

        ourShader.setMat4("model", pclHand.pclList[pclHand.index].model);
        pclHand.pclList[pclHand.index].draw();

        ourShader.setFloat("hole_radius", basic_hole.radius);
        ourShader.setFloat("hole_depth", basic_hole.depth);
        ourShader.setVec3("hole_center", basic_hole.center);
        ourShader.setVec3("cameraPos", camera.Position);

        /*// //OUTPUT FILE WRITTING FOR PYTHON VISUALIZATION
        for(int k = 1; k<pointcloud_list.size() ; k++) {
            pcl_index = k;
            glm::vec4 accel_carla = glm::vec4(imu_data.accel[pcl_index].x, imu_data.accel[pcl_index].y, 0.0f,1.0f);//(8.108274, 0.061310, 0.0, 1.0f);

            glm::vec3 accel_opengl = glm::transpose(imu_carla_to_opengl_coords) * accel_carla  ;

            float dt = imu_data.timestamp[pcl_index] - imu_data.timestamp[pcl_index-1];
            velocity += accel_opengl*dt;

            camera.Position += velocity + 0.5f*accel_opengl*(dt*dt);
            world_to_lidar = glm::translate(world_to_lidar, velocity + 0.5f*accel_opengl*(dt*dt));
            std::ofstream myfile;
            myfile.open(filenames[k]+".txt");
            myfile.write((glm::to_string(camera.Position) + "\n").c_str(), (glm::to_string(camera.Position) + "\n").length());

            std::cout << filenames[k] << "Has: " << pointcloud_list[pcl_index].points.size() << " points"<< std::endl;

            for (int i = 0; i < pointcloud_list[pcl_index].points.size(); i++) {
                //std::cout << "===" << std::endl;
                //if(i!=pcl_index) continue;
                glm::vec3 aPos(pointcloud_list[pcl_index].points[i].x, pointcloud_list[pcl_index].points[i].y,
                               pointcloud_list[pcl_index].points[i].z);
                glm::vec4 world_pos = world_to_lidar * Carla_to_Opengl_coordinates * glm::vec4(aPos, 1.0f);


                if (glm::distance(glm::vec3(world_pos), hole_center) <= hole_radius) {
                    //std::cout << glm::to_string(world_pos) << std::endl;
                    myfile.write((glm::to_string(world_pos) + "\n").c_str(), (glm::to_string(world_pos) + "\n").length());

                    world_pos.y -=
                            hole_depth - (glm::distance(glm::vec3(world_pos), hole_center) / hole_radius) * hole_depth;


                    glm::vec3 camera_to_point_ray = glm::vec3(world_pos) - camera.Position;
                    float modifier = (-2.4 - world_pos.y) / camera_to_point_ray.y;
                    //world_pos.y + x*camera_to_point.y = -2.4 => x = (-2.4 -wolrd_pos.y)/camera_to_point.y;
                    glm::vec3 intersection_point = glm::vec3(world_pos) + camera_to_point_ray * modifier;
                    if (distance(intersection_point, hole_center) > hole_radius) continue;

                }
            }
            myfile.close();
        }
        return 0;//*/

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwPollEvents();
        WindowEventPublisher::notifyFrameUpdate(window, deltaTime);
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void Application::OnKeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        camera.following = !camera.following;
    }

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

void Application::setUpWindowEventHandlers() {
    WindowEventPublisher::addKeyboardListener(camera);
    WindowEventPublisher::addKeyboardListener(*this);
    WindowEventPublisher::addMouseListener(camera);
    WindowEventPublisher::addScrollListener(camera);
    WindowEventPublisher::addFrameUpdateListener(camera);
}

void Application::imGuiDrawWindow(float &hole_radius, float &hole_depth, ImVec4 &clear_color) {
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

    ImGui::Begin("Control");   // Create a window called "Hello, world!" and append into it.

    //ImGui::Text("This is some useful text.");   // Display some text (you can use a format strings too)

    ImGui::SliderFloat("Radius", &hole_radius, 3.0f, 10.0f);
    ImGui::SliderFloat("Depth", &hole_depth, 3.0f, 10.0f);

    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    auto view = camera.GetViewMatrix();
    //ImGui::SameLine();
    ImGui::Text("FrameNumber = %d", pclHand.index);
    ImGui::Text("CameraPos: %f %f %f ", camera.Position[0], camera.Position[1], camera.Position[2]);
    ImGui::Text("Velocity:  %f %f %f ", velocity[0], velocity[1], velocity[2]);
    ImGui::Text("Accel:  %f %f %f    ",  (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pclHand.index+1], 1.0f))[0],
                (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pclHand.index + 1 ], 1.0f))[1],
                (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pclHand.index + 1], 1.0f))[2]);

    ImGui::Text("CameraView: %f %f %f ", view[3][0], view[3][1], view[3][2]);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}


int main()
{
    auto app = Application();

    app.camera = Camera(glm::vec3(0.0f, 0.0f, 2.0f));

    app.window = createGlfwWindow(SCR_WIDTH, SCR_HEIGHT, "CPSoS", false);
    if(app.window == NULL) return -1;
    glfwSetKeyCallback(app.window, [](GLFWwindow* window, int key, int scancode, int action, int mods){ WindowEventPublisher::keyboardCallback(window, key,scancode,action,mods);});
    glfwSetCursorPosCallback(app.window, [](GLFWwindow* window, double xpos, double ypos){ WindowEventPublisher::mouseCallback(window, xpos, ypos);});
    glfwSetScrollCallback(app.window, [](GLFWwindow* window, double xoffset, double yoffset){ WindowEventPublisher::scrollCallback(window, xoffset, yoffset);});
    glfwSetFramebufferSizeCallback(app.window, [](GLFWwindow* window, int width, int height){glViewport(0, 0, width, height);});

    glfwMakeContextCurrent(app.window);

    //glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // Setup Dear ImGui context /////////////////////
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(app.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    app.setUpWindowEventHandlers();
    return app.AppMain();
}
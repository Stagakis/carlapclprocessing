#include "Application.h"
#include "WindowEventPublisher.h"
#include "ShaderLoader.h"
#include "helpers.h"
#include <future>

void loadTexture(std::vector<ImageData>* imgData, const std::string filepath, int i){
    int width, height, nrChannels;
    imgData->operator[](i).data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    imgData->operator[](i).width = width;
    imgData->operator[](i).height = height;
    imgData->operator[](i).nrChannels = nrChannels;
}


int Application::AppMain() {
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    auto files = glob("../resources/*.ply");
    ShaderLoader ourShader("vertexShader.shader", "fragmentShader.shader");

    std::vector<std::future<void>> futures;
    std::vector<ImageData> imgData(files.size()/2);
    for(size_t i = 1; i < files.size()/2; i++) {
        futures.push_back(std::async(std::launch::async, loadTexture, &imgData, files[i].substr(0, files[i].size() - 4) + ".png", i));
    }

    loadTexture(&imgData, files[0].substr(0, files[0].size() - 4) + ".png", 0);
    pointclouds.emplace_back(files[0]);
    images.emplace_back(imgData[0]);
    for(int i = 1; i < files.size()/2 - 1; i++) {
        pointclouds.emplace_back(files[i]);
        futures[i].get();
        images.emplace_back(imgData[i]);
    }
    //pointclouds[0].model = Carla_to_Opengl_coordinates;// * glm::mat4(1.0f);

    for(size_t i = 0 ; i < files.size()/2; i++){
        /*
        glm::vec4 accel_carla = glm::vec4(imu_data.accel[i].x, imu_data.accel[i].y, 0.0f,1.0f);//(8.108274, 0.061310, 0.0, 1.0f);

        glm::vec3 accel_opengl = imu_carla_to_opengl_coords * accel_carla  ;

        float dt = imu_data.timestamp[i] - imu_data.timestamp[i-1];
        velocity += accel_opengl*dt;
        world_to_lidar = glm::translate(world_to_lidar, velocity + 0.5f*accel_opengl*(dt*dt));
        */
        pointclouds[i].translation = glm::vec3(imu_carla_to_opengl_coords * glm::vec4(transformData.lidarPos[i], 1.0f) );
        pointclouds[i].ypr = glm::vec3(imu_carla_to_opengl_coords * glm::vec4(transformData.lidarRot[i], 1.0f) );
        pointclouds[i].updateModelMatrix();
        //pointclouds[i].model = imu_carla_to_opengl_coords * pointclouds[i].model;
    }

    ourShader.use();
    ourShader.setInt("texture0",0);
    ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    while (!glfwWindowShouldClose(window))
    {
        camera.SetFollowingObject(&pointclouds[frameIndex], cameraToLidarOffset);

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

        ourShader.setInt("program_switcher", 0);
        glDisable(GL_DEPTH_TEST);
        images[frameIndex].draw();
        glEnable(GL_DEPTH_TEST);

        ourShader.setInt("program_switcher", 1);
        ourShader.setMat4("model", pointclouds[frameIndex].model * Carla_to_Opengl_coordinates);
        //LOG(glm::to_string(glm::vec3(pointclouds[frameIndex].model * glm::vec4(-0.711443, -7.504014, 2.412690, 1.0f))));
        ourShader.setFloat("hole_radius", holes[0].radius);
        ourShader.setFloat("hole_depth", holes[0].depth);

        ourShader.setVec3("hole_center", holes[0].center);
        ourShader.setVec3("cameraPos", camera.Position);
        pointclouds[frameIndex].draw();

        //*//  //   OUTPUT FILE FOR MATLAB CODE
        for(int k=0 ; k < files.size(); k++) {
        //for(int k=23; k < 24; k++) {
            for (int i = 0; i < holes.size(); i++) {
                applyHole2Pointcloud(pointclouds[k], holes[i]);
            }
            save2obj(files[k].substr(0, files[k].size() - 4) + ".obj", pointclouds[k].points);
        }
        return 0;


        //*/

        /*// //OUTPUT FILE WRITTING FOR PYTHON VISUALIZATION
        for(int k = 1; k<pointclouds.size() ; k++) {
            frameIndex = k;
            glm::vec4 accel_carla = glm::vec4(imu_data.accel[frameIndex].x, imu_data.accel[frameIndex].y, 0.0f,1.0f);//(8.108274, 0.061310, 0.0, 1.0f);

            glm::vec3 accel_opengl = imu_carla_to_opengl_coords * accel_carla  ;

            float dt = imu_data.timestamp[frameIndex] - imu_data.timestamp[frameIndex-1];
            velocity += accel_opengl*dt;

            camera.Position += velocity + 0.5f*accel_opengl*(dt*dt);
            world_to_lidar = glm::translate(world_to_lidar, velocity + 0.5f*accel_opengl*(dt*dt));
            std::ofstream myfile;
            myfile.open(files[k]+"_all.txt");
            myfile.write((glm::to_string(camera.Position) + "\n").c_str(), (glm::to_string(camera.Position) + "\n").length());

            std::cout << files[k] << "Has: " << pointclouds[frameIndex].points.size() << " points"<< std::endl;

            for (int i = 0; i < pointclouds[frameIndex].points.size(); i++) {
                //std::cout << "===" << std::endl;
                //if(i!=pcl_index) continue;
                glm::vec3 aPos(pointclouds[frameIndex].points[i].x, pointclouds[frameIndex].points[i].y,
                               pointclouds[frameIndex].points[i].z);
                glm::vec4 world_pos = world_to_lidar * Carla_to_Opengl_coordinates * glm::vec4(aPos, 1.0f);


                if (glm::distance(glm::vec3(world_pos), basic_hole.center) <= basic_hole.radius) {
                    std::cout << "BEFORE  " << glm::to_string(world_pos) << std::endl;

                    world_pos.y -= basic_hole.depth - (glm::distance(glm::vec3(world_pos), basic_hole.center) / basic_hole.radius) * basic_hole.depth;


                    glm::vec3 camera_to_point_ray = glm::vec3(world_pos) - camera.Position;
                    float modifier = (-2.4 - world_pos.y) / camera_to_point_ray.y;
                    //world_pos.y + x*camera_to_point.y = -2.4 => x = (-2.4 -wolrd_pos.y)/camera_to_point.y;
                    glm::vec3 intersection_point = glm::vec3(world_pos) + camera_to_point_ray * modifier;
                    //myfile.write((glm::to_string(world_pos) + "\n").c_str(), (glm::to_string(world_pos) + "\n").length());

                    if (distance(intersection_point, basic_hole.center) > basic_hole.radius) continue;
                    std::cout << "AFTER   " << glm::to_string(world_pos) << std::endl;

                }
                myfile.write((glm::to_string(world_pos) + "\n").c_str(), (glm::to_string(world_pos) + "\n").length());

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

void Application::initialization() {

    imu_data = CarlaImuParser("../resources/imu.txt");
    transformData = TransformParser("../resources/lidar_cam_metadata.txt");

    auto lidar_rot = imu_carla_to_opengl_coords * glm::vec4(transformData.lidarRot[0], 1.0f);
    auto lidar_pos = glm::eulerAngleYXZ(glm::radians( lidar_rot[0]), glm::radians( lidar_rot[1]), glm::radians( lidar_rot[2]))
                      * imu_carla_to_opengl_coords * glm::vec4(transformData.lidarPos[0], 1.0f);
    auto rgb_rot = imu_carla_to_opengl_coords * glm::vec4(transformData.rgbRot[0], 1.0f);
    auto rgb_pos =  glm::eulerAngleYXZ(glm::radians( rgb_rot[0]), glm::radians( rgb_rot[1]), glm::radians( rgb_rot[2]))
                      * imu_carla_to_opengl_coords * glm::vec4(transformData.rgbPos[0], 1.0f);

    glm::vec4 cameraToLidarOffset_lidarSpace = rgb_pos - lidar_pos;

    cameraToLidarOffset = glm::vec3(cameraToLidarOffset_lidarSpace);
    auto camera_pos = cameraToLidarOffset;
    camera = Camera(glm::vec3(camera_pos));

    //transformData.moveToOrigin();

    basic_hole.radius = 5.0f;
    basic_hole.depth = 1.5;
    basic_hole.center = glm::vec3(0.0f, -2.4f, -18.0f);

    holes.emplace_back(glm::vec3(110.900932, 0.2, -100.492569), 1.8f, 1.5f);
    holes.emplace_back(glm::vec3(106.900932, 0.2, -190.492569), 1.6f, 1.4f);
    holes.emplace_back(glm::vec3(104.900932, 0.2, -80.492569), 1.5f, 1.3f);

    holes.emplace_back(glm::vec3(106.900932, 0.2, -115.492569), 1.8f, 1.5f);
    holes.emplace_back(glm::vec3(104.900932, 0.2, -130.492569), 1.4f, 1.0f);
    holes.emplace_back(glm::vec3(110.900932, 0.2, -144.492569), 1.2f, 1.0f);
    holes.emplace_back(glm::vec3(107.900932, 0.2, -150.492569), 1.0f, 1.0f);
    holes.emplace_back(glm::vec3(106.900932, 0.2, -165.492569), 1.5f, 2.0f);

    //holes.emplace_back(glm::vec3(-6.91f, -1.79f, 56.53), 2.0, 1.2);
    //holes.emplace_back(glm::vec3(-22.0f, -2.52f, 124), 2.0, 1.2);

    //app.holes.emplace_back(glm::vec3(2.0f, -2.4f, -20.0f), 1.5, 2.0);
    //holes.emplace_back(glm::vec3(-1.0f, -2.4f, -10.0f), 1.8, 0.8);

}

void Application::OnKeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        camera.following = !camera.following;
    }

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        //holes[0].center += glm::vec3(imu_carla_to_opengl_coords * glm::vec4(transformData.rgbPos[frameIndex], 1.0f));
        globalCameraPos += transformData.rgbPos[frameIndex];
        frameIndex = std::min(pointclouds.size() - 1, frameIndex + 1);
    }
    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        //holes[0].center -= transformData.rgbPos[frameIndex];
        globalCameraPos -= transformData.rgbPos[frameIndex];
        frameIndex = std::max((size_t)0 ,  frameIndex-1);
    }
}

void Application::imGuiDrawWindow(float &hole_radius, float &hole_depth, ImVec4 &clear_color) {
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

    ImGui::Begin("Control");   // Create a window called "Hello, world!" and append into it.

    //ImGui::Text("This is some useful text.");   // Display some text (you can use a format strings too)
    ImGui::SliderFloat("CameraZoom", &camera.Zoom, 60.0f, 120.0f);
    ImGui::SliderFloat("Radius", &hole_radius, 3.0f, 10.0f);
    ImGui::SliderFloat("Depth", &hole_depth, 3.0f, 10.0f);
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    auto view = camera.GetViewMatrix();
    //ImGui::SameLine();
    ImGui::Text("FrameNumber = %d", frameIndex);
    ImGui::Text("CameraPos: %f %f %f ", camera.Position[0], camera.Position[1], camera.Position[2]);
    ImGui::Text("transformData.rgbPos: %f %f %f ", transformData.rgbPos[frameIndex][0], transformData.rgbPos[frameIndex][1], transformData.rgbPos[frameIndex][2]);
    ImGui::Text("GobalCamPos: %f %f %f ", globalCameraPos[0], globalCameraPos[1], globalCameraPos[2]);

    ImGui::Text("hole[0] Center: %f %f %f ", holes[0].center[0], holes[0].center[1], holes[0].center[2]);

    ImGui::Text("Velocity:  %f %f %f ", velocity[0], velocity[1], velocity[2]);
    ImGui::Text("Accel:  %f %f %f    ",  (imu_carla_to_opengl_coords * glm::vec4(imu_data.accel[frameIndex], 1.0f))[0],
                (imu_carla_to_opengl_coords * glm::vec4(imu_data.accel[frameIndex], 1.0f))[1],
                (imu_carla_to_opengl_coords * glm::vec4(imu_data.accel[frameIndex], 1.0f))[2]);

    ImGui::Text("CameraView: %f %f %f ", view[3][0], view[3][1], view[3][2]);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}


void Application::setUpWindowEventHandlers() {
    WindowEventPublisher::addKeyboardListener(camera);
    WindowEventPublisher::addKeyboardListener(*this);
    WindowEventPublisher::addMouseListener(camera);
    WindowEventPublisher::addScrollListener(camera);
    WindowEventPublisher::addFrameUpdateListener(camera);
}

int main()
{
    auto app = Application();

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

    app.initialization();
    return app.AppMain();
}
#include "Application.h"
#include "WindowEventPublisher.h"
#include "ShaderLoader.h"
#include "PointcloudHandler.h"


int Application::AppMain() {
    basic_hole.radius = 2.0f;
    basic_hole.depth = 1.5;
    basic_hole.center = glm::vec3(0.0f, -2.4f, -18.0f);

    glEnable(GL_DEPTH_TEST);
    ShaderLoader ourShader("vertexShader.shader", "fragmentShader.shader");

    imu_data = CarlaImuParser("../resources/imu.txt");
    std::vector<Pointcloud> pointcloud_list;
    for(const auto& file : glob("../resources/*.ply"))
        pointcloud_list.emplace_back(file);
    pclHand = PointcloudHandler(pointcloud_list);

    pclHand.pclList[0].model = glm::mat4(1.0f)*Carla_to_Opengl_coordinates;
    for(int i = 1 ; i<pclHand.pclList.size(); i++){
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

bool Application::initializeContext(const std::string &windowName, bool fullscreen) {
    winHandler = new WindowHandler(SCR_WIDTH, SCR_HEIGHT, windowName, fullscreen);
    window = winHandler->GetGLFWwindowPtr();

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods){ WindowEventPublisher::keyboardCallback(window, key,scancode,action,mods);});
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos){ WindowEventPublisher::mouseCallback(window, xpos, ypos);});
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset){ WindowEventPublisher::scrollCallback(window, xoffset, yoffset);});

    WindowEventPublisher::addKeyboardListener(*winHandler);

    return true;
}

void Application::initializeImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::setUpWindowEventHandlers() {
    WindowEventPublisher::addKeyboardListener(camera);
    WindowEventPublisher::addKeyboardListener(Application());
    WindowEventPublisher::addMouseListener(camera);
    WindowEventPublisher::addScrollListener(camera);
    WindowEventPublisher::addFrameUpdateListener(camera);

}

std::vector<std::string> Application::glob(const std::string &pattern) {
    using namespace std;

    // glob struct resides on the stack
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    // do the glob operation
    int return_value = ::glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    if(return_value != 0) {
        globfree(&glob_result);
        stringstream ss;
        ss << "glob() failed with return_value " << return_value << endl;
        throw std::runtime_error(ss.str());
    }

    // collect all the filenames into a std::list<std::string>
    vector<string> filenames;
    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        filenames.push_back(string(glob_result.gl_pathv[i]));
    }

    // cleanup
    globfree(&glob_result);

    // done
    return filenames;
}

void Application::imGuiDrawWindow(float &hole_radius, float &hole_depth, ImVec4 &clear_color) {
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

    ImGui::SliderFloat("Radius", &hole_radius, 3.0f, 10.0f);
    ImGui::SliderFloat("Depth", &hole_depth, 3.0f, 10.0f);

    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    auto view = camera.GetViewMatrix();
    ImGui::SameLine();
    ImGui::Text("FrameNumber = %d", pcl_index);
    ImGui::Text("CameraPos: %f %f %f ", camera.Position[0], camera.Position[1], camera.Position[2]);
    ImGui::Text("Velocity:  %f %f %f ", velocity[0], velocity[1], velocity[2]);
    ImGui::Text("Accel:  %f %f %f    ",  (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pcl_index+1], 1.0f))[0],
                (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pcl_index + 1 ], 1.0f))[1],
                (glm::transpose(imu_carla_to_opengl_coords) * glm::vec4(imu_data.accel[pcl_index + 1], 1.0f))[2]);

    ImGui::Text("CameraView: %f %f %f ", view[3][0], view[3][1], view[3][2]);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void Application::OnKeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        camera.following = !camera.following;
    }
}

int main()
{

    Application::initializeContext("Test", false);
    Application::initializeImGui();
    Application::setUpWindowEventHandlers();
    return Application::AppMain();
}
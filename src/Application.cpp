
#include "Application.h"
#include "WindowEventPublisher.h"
#include "ShaderLoader.h"
#include "helpers.h"
#include <future>


int Application::AppMain() {
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    auto files = glob("../resources/*.ply");
    //auto files = glob("../resources/*.obj");
    ShaderLoader ourShader("vertexShader.shader", "fragmentShader.shader");
    ShaderLoader postProcessShader("vertexShader.shader", "KNearest_fragment.shader");

    std::vector<std::future<void>> futures;
    std::vector<ImageData> imgData(files.size()/2 );
    for(size_t i = 1; i < files.size()/2 ; i++) {
        futures.push_back(std::async(std::launch::async, loadTexture, &imgData, files[i].substr(0, files[i].size() - 4) + ".png", i));
        //futures.push_back(std::async(std::launch::async, loadTexture, &imgData, files[i].substr(0, files[i].size() - std::string("_saliency_binary.obj").size()) + ".png", i));

    }
    loadTexture(&imgData, files[0].substr(0, files[0].size() - 4)  + ".png", 0);
    //loadTexture(&imgData, files[0].substr(0, files[0].size() - std::string("_saliency_binary.obj").size())  + ".png", 0);

    pointclouds.emplace_back(files[0]);
    images.emplace_back(imgData[0]);
    for(int i = 1; i < files.size()/2 - 1; i++) {
        pointclouds.emplace_back(files[i]);
        futures[i].get();
        images.emplace_back(imgData[i]);
    }

    for(size_t i = 0 ; i < files.size()/2; i++){
        pointclouds[i].translation = glm::vec3(imu_carla_to_opengl_coords * glm::vec4(transformData.lidarPos[i], 1.0f) );
        pointclouds[i].ypr = glm::vec3(-transformData.lidarRot[i][1], transformData.lidarRot[i][0], transformData.lidarRot[i][2]);
        pointclouds[i].updateModelMatrix();
    }

    ourShader.use();
    ourShader.setInt("texture0",0);
    ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    auto pcl = Pointcloud("../004489_saliency_binary.obj");
    while (!glfwWindowShouldClose(window))
    //for(;frameIndex < files.size();frameIndex++)
    {
        cameraToLidarOffset = imu_carla_to_opengl_coords * glm::vec4(transformData.rgbPos[frameIndex] - transformData.lidarPos[frameIndex] , 1.0f);
        //LOG(glm::to_string(cameraToLidarOffset));
        camera.SetFollowingObject(&pointclouds[frameIndex], cameraToLidarOffset);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        imGuiDrawWindow(basic_hole.radius, basic_hole.depth, clear_color);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

        //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        pointclouds[frameIndex].draw();
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /*//
        //POST PROCESSING
        postProcessShader.use();
        postProcessShader.setInt("program_switcher", 0);
        postProcessShader.setVec2("stepSize", 1.0f/SCR_WIDTH, 1.0f/SCR_HEIGHT);
        postProcessShader.setFloat("alpha_value", 1.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDisable(GL_DEPTH_TEST);
        //glDisable(GL_BLEND);

        //images[frameIndex].draw(fbTexture);
        //images[frameIndex].draw(fbTexture);
        //images[frameIndex].draw(fbTexture);
        //images[frameIndex].draw(fbTexture);
        //images[frameIndex].draw(fbTexture);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_BLEND);
        postProcessShader.setFloat("alpha_value", 0.5f);

        images[frameIndex].draw(fbTexture);
        glEnable(GL_DEPTH_TEST);
        //saveFrame(frameIndex, 4, window);
        //*/

        /*//  //   OUTPUT FILE FOR MATLAB CODE
        for(int k=0 ; k < files.size(); k++) {
        //for(int k=23; k < 24; k++) {
            for (int i = 0; i < holes.size(); i++) {
                applyHole2Pointcloud(pointclouds[k], holes[i]);
            }
            save2obj(files[k].substr(0, files[k].size() - 4) + ".obj", pointclouds[k].points);
        }
        return 0;
        //*/

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



    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &fbTexture);
    glBindTexture(GL_TEXTURE_2D, fbTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1600, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    imu_data = CarlaImuParser("../resources/imu.txt");
    transformData = TransformParser("../resources/lidar_cam_metadata.txt");

    camera = Camera();

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

}

void Application::OnKeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        camera.following = !camera.following;
    }
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        frameIndex = std::min(pointclouds.size() - 1, frameIndex + 1);
    }
    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        frameIndex = std::max((size_t)0 ,  frameIndex-1);
    }
}

void Application::imGuiDrawWindow(float &hole_radius, float &hole_depth, ImVec4 &clear_color) {
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

    ImGui::Begin("Control");   // Create a window called "Hello, world!" and append into it.

    //ImGui::Text("This is some useful text.");   // Display some text (you can use a format strings too)
    ImGui::SliderFloat("CameraZoom", &camera.Zoom, 40.0f, 110.0f);
    ImGui::SliderFloat("Radius", &hole_radius, 3.0f, 10.0f);
    ImGui::SliderFloat("Depth", &hole_depth, 3.0f, 10.0f);
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    //ImGui::SameLine();
    ImGui::Text("FrameNumber = %ld", frameIndex);
    ImGui::Text("CameraPos: %f %f %f ", camera.Position[0], camera.Position[1], camera.Position[2]);
    ImGui::Text("CameraFront: %f %f %f ", camera.Front[0], camera.Front[1], camera.Front[2]);
    ImGui::Text("CameraYP: %f %f  ", camera.Yaw, camera.Pitch);
    ImGui::Text("PclYPR: %f %f %f ", pointclouds[frameIndex].ypr[0], pointclouds[frameIndex].ypr[1],pointclouds[frameIndex].ypr[2]);

    ImGui::Text("transformData.rgbPos: %f %f %f ", transformData.rgbPos[frameIndex][0], transformData.rgbPos[frameIndex][1], transformData.rgbPos[frameIndex][2]);

    /*
    auto view = camera.GetViewMatrix();

    ImGui::Text("GobalCamPos: %f %f %f ", globalCameraPos[0], globalCameraPos[1], globalCameraPos[2]);

    ImGui::Text("hole[0] Center: %f %f %f ", holes[0].center[0], holes[0].center[1], holes[0].center[2]);
    ImGui::Text("Velocity:  %f %f %f ", velocity[0], velocity[1], velocity[2]);
    ImGui::Text("Accel:  %f %f %f    ",  (imu_carla_to_opengl_coords * glm::vec4(imu_data.accel[frameIndex], 1.0f))[0],
                (imu_carla_to_opengl_coords * glm::vec4(imu_data.accel[frameIndex], 1.0f))[1],
                (imu_carla_to_opengl_coords * glm::vec4(imu_data.accel[frameIndex], 1.0f))[2]);

    ImGui::Text("CameraView: %f %f %f ", view[3][0], view[3][1], view[3][2]);
     */
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
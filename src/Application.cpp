#include "Application.h"
#include "WindowEventPublisher.h"
#include "ShaderLoader.h"
#include "helpers.h"
#include <future>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <execution>
#include <utility>

namespace fs = std::filesystem;

int Application::AppMain() {
    ShaderLoader ourShader("vertexShader.shader", "fragmentShader.shader");
    ShaderLoader postProcessShader("vertexShader.shader", "KNearest_fragment.shader");

    ourShader.use();
    ourShader.setInt("texture0",0);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window))
    {
        cameraToLidarOffset = imu_carla_to_opengl_coords * glm::vec4(transformData.rgbPos[frameIndex] - transformData.lidarPos[frameIndex] , 1.0f);
        camera.SetFollowingObject(&pointclouds[frameIndex], cameraToLidarOffset);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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

        ourShader.setVec3("cameraPos", camera.Position);

        //POST PROCESSING
        if(usePostprocessing) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            pointclouds[frameIndex].draw();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            postProcessShader.use();
            postProcessShader.setInt("program_switcher", 0);
            postProcessShader.setVec2("stepSize", 1.0f / SCR_WIDTH, 1.0f / SCR_HEIGHT);
            postProcessShader.setFloat("alpha_value", 1.0f);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            for(int i = 0; i < iterNumber ; ++i)
                images[frameIndex].draw(fbTexture);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_BLEND);
            postProcessShader.setFloat("alpha_value", 0.5f);

            images[frameIndex].draw(fbTexture);
            glEnable(GL_DEPTH_TEST);
        }
        else
            pointclouds[frameIndex].draw();

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

        imgui.draw();
        glfwPollEvents();
        WindowEventPublisher::notifyFrameUpdate(window, deltaTime);

        if(recording){
            saveFrame(frameIndex, 5, window);
            frameIndex++;
            if(frameIndex==pointclouds.size()) break;
        }
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void Application::Initialization() {

    std::string resources_folder = "../resources_ego0/";
    
    /*
    std::ifstream inFile;
    inFile.open(resources_folder + "occupancy_ego1.csv");
    std::string line;
    for (; std::getline(inFile, line);){
        occupancyFactor.push_back(stof(line.substr(line.find(',') + 1, std::string::npos)));
    }
    */

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

    //imu_data = ImuParser("../resources/imu.txt");
    transformData = TransformParser(resources_folder + "camera_metadata.txt", resources_folder + "lidar_metadata.txt");
    steeringData = SteeringParser(resources_folder + "steering_true.txt");
    camera = Camera();


    //TEXTURE LOADING
    stbi_set_flip_vertically_on_load(true);

    std::vector<std::string> file_paths;
    std::vector<std::string> file_namestems;

    for (auto& p : fs::recursive_directory_iterator(resources_folder))
    {
        if (p.path().extension() == ext) {
            file_paths.push_back(p.path().string());
            file_namestems.push_back(p.path().stem().string());
        }
        
    }

    std::vector<std::string> files = glob(resources_folder + "*_saliency_segmentation.obj");
    std::vector<std::string> image_files = glob(resources_folder + "*.png");

    /*//
    std::vector<std::string> files_halved;
    for(int i = 0 ; i < files.size() - 150; i++){
        files_halved.push_back(files[i]);
    }
    files = files_halved;
    //*/

    auto obj_name_ending = std::string("_saliency_segmentation");
    std::vector<std::future<void>> futures;
    std::vector<ImageData> imgData(files.size() );
    std::cout << "Loading images........." << "\n";
    const int batch_size = 100;
    for(int batch = 0; batch < files.size()/batch_size; batch++ ) {
        for (size_t i = batch_size*batch; i < batch_size*(batch + 1); i++) {
            futures.push_back(std::async(std::launch::async, loadTexture, &imgData, image_files[i], i));
        }
        for(int i = batch_size*batch; i < batch_size*(batch + 1); i++) {
            futures[i].get();
            images.emplace_back(imgData[i]);
        }
    }
    std::cout << "Finished loading images" << "\n";

    std::cout << "Loading OBJs........." << "\n";
    pointclouds.resize(files.size());
    std::transform(std::execution::par_unseq, files.begin(), files.end(), pointclouds.begin(),
                   [](std::string file) -> Pointcloud { return Pointcloud(std::move(file)); });
    std::cout << "Finished loading OBJs" << "\n";

    //PREPROCESSING done if the obj is rotated from the steering
    for(size_t i = 0 ; i < files.size() ; i++) {
        pointclouds[i].applyYaw(steeringData.angles[i]);
        pointclouds[i].sendDataToGPU();
    }

    for(size_t i = 0 ; i < files.size(); i++){
        pointclouds[i].translation = glm::vec3(imu_carla_to_opengl_coords * glm::vec4(transformData.lidarPos[i], 1.0f) );

        pointclouds[i].ypr = glm::vec3(-transformData.lidarRot[i][1], transformData.lidarRot[i][0], -transformData.lidarRot[i][2]); // roll is minus because we look at the -z axis
        pointclouds[i].updateModelMatrix();
    }
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
        frameIndex = (size_t)std::max(0 , (int(frameIndex))-1);
    }
}

void Application::setUpWindowEventHandlers() {
    WindowEventPublisher::addKeyboardListener(camera);
    WindowEventPublisher::addKeyboardListener(*this);
    WindowEventPublisher::addMouseListener(camera);
    WindowEventPublisher::addScrollListener(camera);
    WindowEventPublisher::addFrameUpdateListener(camera);
}

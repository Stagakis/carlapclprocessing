#include "Application.h"
#include "Singletons/WindowEventPublisher.h"
#include "ShaderLoader.h"
#include "helpers.h"
#include <iostream>
#include <algorithm>
#include "Ego.h"

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
        auto& vehicle = vehicles[active_vehicle];
        auto lidarTransform = vehicle.data.get_lidar_transformation();
        auto cameraTransform = vehicle.data.get_camera_transformation();

        cameraToLidarOffset = unreal_to_opengl_coord_system * glm::vec4(cameraTransform.first - lidarTransform.first , 1.0f);
        camera.SetFollowingObject(&vehicle.data.get_pointcloud(), cameraToLidarOffset);

        for(auto& mvehicle: vehicles){
            mvehicle.checkForObstacles(frameIndex, 0.0f);
        }

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
        vehicle.data.get_image().draw();
        glEnable(GL_DEPTH_TEST);

        ourShader.setInt("program_switcher", 1);
        ourShader.setMat4("model", vehicle.data.get_pointcloud().model);
        //LOG(glm::to_string(glm::vec3(pointclouds[frameIndex].model * glm::vec4(-0.711443, -7.504014, 2.412690, 1.0f))));
        ourShader.setVec3("cameraPos", camera.Position);

        //POST PROCESSING
        if(usePostprocessing) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            vehicle.data.get_pointcloud().draw();
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
            postProcessShader.use();
            postProcessShader.setInt("program_switcher", 0);
            postProcessShader.setVec2("stepSize", 1.0f / SCR_WIDTH, 1.0f / SCR_HEIGHT);
            postProcessShader.setFloat("alpha_value", 1.0f);
            //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            for(int i = 0; i < iterNumber ; ++i)
                vehicle.data.get_image().draw(fbTexture);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_BLEND);
            postProcessShader.setFloat("alpha_value", 0.5f);

            vehicle.data.get_image().draw(fbTexture);
            glEnable(GL_DEPTH_TEST);
        }
        else {
            auto obstacles = Server::GetRelevantObstacles(lidarTransform.first, lidarTransform.second);
            vehicle.data.get_pointcloud().draw();

            for (auto& obst : obstacles) {
                vehicle.handleObstacle(*obst);
                ourShader.setMat4("model", obst->pcl.model);
                obst->pcl.draw();
            }
        }
        ImguiManager::DrawAllWindows();
        glfwPollEvents();
        WindowEventPublisher::notifyFrameUpdate(window, deltaTime);

        if(recording){
            saveFrame(frameIndex, 5, window);
            frameIndex++;
            if(frameIndex==vehicle.data.pointclouds.size()) break;
        }
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

void Application::Initialization() {
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
    //TEXTURE LOADING
    stbi_set_flip_vertically_on_load(true);

    camera = Camera();

    vehicles.emplace_back("../resources_ego0/");
    //vehicles.emplace_back("../resources_ego1/");
}

void Application::OnKeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        camera.following = !camera.following;
    }
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        frameIndex = std::min(vehicles[active_vehicle].data.pointclouds.size() - 1, frameIndex + 1);
        SensorData::frameIndex = frameIndex;
    }
    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        frameIndex = (size_t)std::max(0 , (int(frameIndex))-1);
        SensorData::frameIndex = frameIndex;
    }
    if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS){
        active_vehicle = (active_vehicle + 1)%vehicles.size(); //Cycle through the available vehicles
    }
}

void Application::setUpWindowEventHandlers() {
    WindowEventPublisher::addKeyboardListener(camera);
    WindowEventPublisher::addKeyboardListener(*this);
    WindowEventPublisher::addMouseListener(camera);
    WindowEventPublisher::addScrollListener(camera);
    WindowEventPublisher::addFrameUpdateListener(camera);
}

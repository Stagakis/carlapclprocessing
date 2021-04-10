//
// Created by stagakis on 8/2/21.
//

#include "Singletons/ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Application.h"
#include "helpers.h"

Application* ImguiManager::app = nullptr;

void ImguiManager::InitializeImguiManager(Application& _app) {
    app = &_app;

    // Setup Dear ImGui context /////////////////////
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(app->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImguiManager::imGuiDrawWindow() {
    auto& vehicle = app->vehicles[app->active_vehicle];

    ImGui::Begin("Control");   // Create a window called "Hello, world!" and append into it.

    ImGui::SliderFloat("CameraZoom", &app->camera.Zoom, 40.0f, 110.0f);
    ImGui::ColorEdit3("clear color", (float*)&app->clear_color); // Edit 3 floats representing a color

    ImGui::Text("FrameNumber = %ld", app->frameIndex);
    ImGui::Text("CameraPos: %f %f %f ", app->camera.Position[0], app->camera.Position[1], app->camera.Position[2]);
    ImGui::Text("CameraFront: %f %f %f ", app->camera.Front[0], app->camera.Front[1], app->camera.Front[2]);
    ImGui::Text("CameraYP: %f %f  ", app->camera.Yaw, app->camera.Pitch);
    ImGui::Text("LidarSensorYPR: %f %f %f ", vehicle.pointclouds[app->frameIndex].ypr[0], vehicle.pointclouds[app->frameIndex].ypr[1], vehicle.pointclouds[app->frameIndex].ypr[2]);
    ImGui::Text("LidarSensorTranslationVector: %f %f %f ", vehicle.pointclouds[app->frameIndex].translation[0], vehicle.pointclouds[app->frameIndex].translation[1], vehicle.pointclouds[app->frameIndex].translation[2]);

    ImGui::Checkbox("PostProcessing", &app->usePostprocessing);
    ImGui::Checkbox("StartRecording", &app->recording);

    if(ImGui::Button("TakeSnapshot")){
       saveFrame(app->frameIndex, 5, app->window);
    }

    if(app->usePostprocessing){
        //ImGui::SameLine();
        ImGui::InputScalar("IterNumber",      ImGuiDataType_S8,     &app->iterNumber, &app->iterNumberStep, NULL, "%d");
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void ImguiManager::imGuiOccupancyFactor() {

    ImGui::Begin("Help");   // Create a window called "Hello, world!" and append into it.

    //ImGui::Text("OccupancyFactor = %f", app->occupancyFactor[app->frameIndex]);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Dangerous area");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Be-aware area");
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "semi dangerous area");
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Offroad");
    ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Safe area");
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "No Idea");

    ImGui::End();

}

void ImguiManager::DrawAllWindows() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //Draw windows
    imGuiDrawWindow();
    //oImGui::ShowDemoWindow();
    imGuiOccupancyFactor();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

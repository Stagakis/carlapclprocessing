//
// Created by stagakis on 8/2/21.
//

#include "ImguiManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Application.h"
#include "helpers.h"



ImguiManager::ImguiManager(Application & app): _app(app) {
}


void ImguiManager::imGuiDrawWindow(ImVec4 &clear_color) {
    auto& vehicle = _app.vehicles[_app.active_vehicle];

    ImGui::Begin("Control");   // Create a window called "Hello, world!" and append into it.

    ImGui::SliderFloat("CameraZoom", &_app.camera.Zoom, 40.0f, 110.0f);
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    ImGui::Text("FrameNumber = %ld", _app.frameIndex);
    ImGui::Text("CameraPos: %f %f %f ", _app.camera.Position[0], _app.camera.Position[1], _app.camera.Position[2]);
    ImGui::Text("CameraFront: %f %f %f ", _app.camera.Front[0], _app.camera.Front[1], _app.camera.Front[2]);
    ImGui::Text("CameraYP: %f %f  ", _app.camera.Yaw, _app.camera.Pitch);
    ImGui::Text("PclYPR: %f %f %f ", vehicle.pointclouds[_app.frameIndex].ypr[0], vehicle.pointclouds[_app.frameIndex].ypr[1], vehicle.pointclouds[_app.frameIndex].ypr[2]);

    ImGui::Text("transformData.rgbPos: %f %f %f ", vehicle.transformData.rgbPos[_app.frameIndex][0], vehicle.transformData.rgbPos[_app.frameIndex][1], vehicle.transformData.rgbPos[_app.frameIndex][2]);

    ImGui::Checkbox("PostProcessing", &_app.usePostprocessing);
    ImGui::Checkbox("StartRecording", &_app.recording);

    if(ImGui::Button("TakeSnapshot")){
        //saveFrame(_app.frameIndex, 5, _app.window);
    }

    if(_app.usePostprocessing){
        //ImGui::SameLine();
        ImGui::InputScalar("IterNumber",      ImGuiDataType_S8,     &_app.iterNumber, &_app.iterNumberStep, NULL, "%d");
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void ImguiManager::imGuiOccupancyFactor() {
    ImGui::Begin("Help");   // Create a window called "Hello, world!" and append into it.

    //ImGui::Text("OccupancyFactor = %f", _app.occupancyFactor[_app.frameIndex]);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Dangerous area");
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Be-aware area");
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "semi dangerous area");
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Offroad");
    ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Safe area");
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "No Idea");

    ImGui::End();

}

void ImguiManager::draw() {
    // Rendering
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    imGuiDrawWindow(_app.clear_color);
    //oImGui::ShowDemoWindow();
    imGuiOccupancyFactor();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


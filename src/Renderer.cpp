#include "Renderer.h"

void Renderer::OnFrameUpdate(GLFWwindow* window, float deltaTime) {

}

void Renderer::OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void Renderer::OnMouseEvent(GLFWwindow* window, double xpos, double ypos) {

}

void Renderer::OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {

}

void Renderer::InitializeContext() {

}

void Renderer::MainLoop() {

}

Renderer::Renderer(GLFWwindow *_window, Camera& _camera) {
    window = _window;
    camera = _camera;
}

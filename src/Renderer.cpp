#include "Renderer.h"

void Renderer::OnFrameUpdate() {

}

void Renderer::OnKeyboardEvent(int key, int scancode, int action, int mods) {

}

void Renderer::OnMouseEvent(double xpos, double ypos) {

}

void Renderer::OnScrollEvent(double xoffset, double yoffset) {

}

void Renderer::InitializeContext() {

}

void Renderer::MainLoop() {

}

Renderer::Renderer(GLFWwindow *_window, Camera& _camera) {
    window = _window;
    camera = _camera;
}

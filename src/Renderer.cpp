#include "Renderer.h"

void Renderer::OnFrameUpdate(GLFWwindow* window, float deltaTime) {

}

void Renderer::OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void Renderer::OnMouseEvent(GLFWwindow* window, double xpos, double ypos) {

}

void Renderer::OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {

}

void Renderer::InitializeContext(int width, int height, const std::string& windowName,bool fullscreen) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(width, height, windowName.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height){glViewport(0, 0, width, height);});
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Renderer::MainLoop() {

}

Renderer::Renderer(GLFWwindow *_window, Camera& _camera) {
    window = _window;
    camera = _camera;
}

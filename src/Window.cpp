#include "Window.h"
#include <iostream>

//Initialization of static variables
GLFWwindow* Window::window = nullptr;
std::map<key_comb, callback_function> Window::key_callbacks = std::map<key_comb, callback_function>();

int Window::CreateNewWindow(int width, int height, const std::string& windowName,bool fullscreen) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, windowName.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    return 1;
}

void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
}

void Window::mouse_callback(GLFWwindow *window, double xpos, double ypos) {

}

void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {

}

void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    key_callbacks[key_comb(key,mods)]();
}

void Window::swap_buffers() {
    glfwSwapBuffers(window);
}

void Window::add_key_callback(int key, callback_function func, int mods) {
    key_callbacks.insert(std::make_pair(key_comb(key, mods), func));
}


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

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(width, height, windowName.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height){glViewport(0, 0, width, height);});
    glfwSetKeyCallback(window, AllKeyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return 1;
}

void Window::AllKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto func = key_callbacks[key_comb(key, action, mods)];
    if(func != nullptr) func();
}


void Window::AddKeyCallback(int key, int action, callback_function func, int mods) {
    key_callbacks.insert(std::make_pair(key_comb(key, action, mods), func));
}

bool Window::CheckKey(int key, int state) {
    return glfwGetKey(window, key) == state;

}

void Window::FinishFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}



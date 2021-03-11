#include "Singletons/WindowEventPublisher.h"

std::vector<IWindowEventListener*> WindowEventPublisher::keyboardListeners = std::vector<IWindowEventListener*>();
std::vector<IWindowEventListener*> WindowEventPublisher::mouseListeners = std::vector<IWindowEventListener*>();
std::vector<IWindowEventListener*> WindowEventPublisher::scrollListeners = std::vector<IWindowEventListener*>();
std::vector<IWindowEventListener*> WindowEventPublisher::frameUpdateListeners = std::vector<IWindowEventListener*>();


//*//
void WindowEventPublisher::addKeyboardListener(IWindowEventListener& listener) {
    keyboardListeners.push_back(&listener);
}
//*/

void WindowEventPublisher::addMouseListener(IWindowEventListener&  listener) {
    mouseListeners.push_back(&listener);
}

void WindowEventPublisher::addScrollListener(IWindowEventListener&  listener) {
    scrollListeners.push_back(&listener);
}

void WindowEventPublisher::addFrameUpdateListener(IWindowEventListener &listener) {
    frameUpdateListeners.push_back(&listener);
}

void WindowEventPublisher::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    for(auto & keyboardListener : keyboardListeners){
        keyboardListener->OnKeyboardEvent(window, key, scancode, action, mods);
    }
}

void WindowEventPublisher::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    for(auto & mouseListener : mouseListeners){
        mouseListener->OnMouseEvent(window, xpos, ypos);
    }
}

void WindowEventPublisher::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    for(auto & scrollListener : scrollListeners){
        scrollListener->OnScrollEvent(window, xoffset, yoffset);
    }
}

void WindowEventPublisher::notifyFrameUpdate(GLFWwindow* window, float deltaTime) {
    for(auto & frameUpdateListener : frameUpdateListeners){
        frameUpdateListener->OnFrameUpdate(window, deltaTime);
    }
}


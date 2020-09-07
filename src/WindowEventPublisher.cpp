#include "WindowEventPublisher.h"

std::vector<IntWindowEventListener*> WindowEventPublisher::keyboardListeners = std::vector<IntWindowEventListener*>();
std::vector<IntWindowEventListener*> WindowEventPublisher::mouseListeners = std::vector<IntWindowEventListener*>();
std::vector<IntWindowEventListener*> WindowEventPublisher::scrollListeners = std::vector<IntWindowEventListener*>();
std::vector<IntWindowEventListener*> WindowEventPublisher::frameUpdateListeners = std::vector<IntWindowEventListener*>();


//*//
void WindowEventPublisher::addKeyboardListener(IntWindowEventListener& listener) {
    keyboardListeners.push_back(&listener);
}
//*/

void WindowEventPublisher::addMouseListener(IntWindowEventListener&  listener) {
    mouseListeners.push_back(&listener);
}

void WindowEventPublisher::addScrollListener(IntWindowEventListener&  listener) {
    scrollListeners.push_back(&listener);
}

void WindowEventPublisher::addFrameUpdateListener(IntWindowEventListener &listener) {
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


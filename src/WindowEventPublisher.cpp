//
// Created by stagakis on 4/9/20.
//

#include "WindowEventPublisher.h"

std::vector<IntWindowEventListener*> WindowEventPublisher::keyboardListeners = std::vector<IntWindowEventListener*>();
std::vector<IntWindowEventListener*> WindowEventPublisher::mouseListeners = std::vector<IntWindowEventListener*>();
std::vector<IntWindowEventListener*> WindowEventPublisher::scrollListeners = std::vector<IntWindowEventListener*>();


void WindowEventPublisher::addKeyboardListener(IntWindowEventListener& listener) {
    keyboardListeners.push_back(&listener);
}

void WindowEventPublisher::addMouseListener(IntWindowEventListener* listener) {
    mouseListeners.push_back(listener);
}

void WindowEventPublisher::addScrollListener(IntWindowEventListener* listener) {
    scrollListeners.push_back(listener);
}

void WindowEventPublisher::keyboardCallback(int key, int scancode, int action, int mods) {
    for(auto & keyboardListener : keyboardListeners){
        keyboardListener->OnKeyboardEvent(key, scancode, action, mods);
    }
}

void WindowEventPublisher::mouseCallback(double xpos, double ypos) {
    for(auto & mouseListener : mouseListeners){
        mouseListener->OnMouseEvent(xpos, ypos);
    }
}

void WindowEventPublisher::scrollCallback(double xoffset, double yoffset) {
    for(auto & scrollListener : scrollListeners){
        scrollListener->OnScrollEvent(xoffset, yoffset);
    }
}

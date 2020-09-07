#ifndef MYOPENGL_WINDOWEVENTPUBLISHER_H
#define MYOPENGL_WINDOWEVENTPUBLISHER_H

#include "IntWindowEventListener.h"
#include <vector>
#include <iostream>

class WindowEventPublisher {

public:
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static void notifyFrameUpdate(GLFWwindow* window, float deltaTime);

    static void addKeyboardListener(IntWindowEventListener& listener);
    static void addFrameUpdateListener(IntWindowEventListener& listener);
    static void addMouseListener(IntWindowEventListener&  listener);
    static void addScrollListener(IntWindowEventListener&  listener);

private:
    static std::vector<IntWindowEventListener*> keyboardListeners;
    static std::vector<IntWindowEventListener*> frameUpdateListeners;
    static std::vector<IntWindowEventListener*> mouseListeners;
    static std::vector<IntWindowEventListener*> scrollListeners;

};


#endif //MYOPENGL_WINDOWEVENTPUBLISHER_H

#ifndef MYOPENGL_WINDOWEVENTPUBLISHER_H
#define MYOPENGL_WINDOWEVENTPUBLISHER_H

#include "IWindowEventListener.h"
#include <vector>
#include <iostream>

class WindowEventPublisher {

public:
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    static void notifyFrameUpdate(GLFWwindow* window, float deltaTime);

    static void addKeyboardListener(IWindowEventListener& listener);
    static void addFrameUpdateListener(IWindowEventListener& listener);
    static void addMouseListener(IWindowEventListener&  listener);
    static void addScrollListener(IWindowEventListener&  listener);

private:
    static std::vector<IWindowEventListener*> keyboardListeners;
    static std::vector<IWindowEventListener*> frameUpdateListeners;
    static std::vector<IWindowEventListener*> mouseListeners;
    static std::vector<IWindowEventListener*> scrollListeners;

};


#endif //MYOPENGL_WINDOWEVENTPUBLISHER_H

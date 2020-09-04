#ifndef MYOPENGL_WINDOWEVENTPUBLISHER_H
#define MYOPENGL_WINDOWEVENTPUBLISHER_H

#include "IntWindowEventListener.h"
#include <vector>

class WindowEventPublisher {

public:

    static std::vector<IntWindowEventListener*> keyboardListeners;
    static std::vector<IntWindowEventListener*> mouseListeners;
    static std::vector<IntWindowEventListener*> scrollListeners;

    static void keyboardCallback(int key, int scancode, int action, int mods);
    static void mouseCallback(double xpos, double ypos);
    static void scrollCallback(double xoffset, double yoffset);

    static void addKeyboardListener(IntWindowEventListener& listener);
    static void addMouseListener(IntWindowEventListener* listener);
    static void addScrollListener(IntWindowEventListener* listener);
};


#endif //MYOPENGL_WINDOWEVENTPUBLISHER_H

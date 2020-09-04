//
// Created by stagakis on 4/9/20.
//

#ifndef MYOPENGL_INTWINDOWEVENTLISTENER_H
#define MYOPENGL_INTWINDOWEVENTLISTENER_H

#include <GLFW/glfw3.h>

class IntWindowEventListener {
    virtual void OnKeyboardEvent(int key, int scancode, int action, int mods) = 0;
    virtual void OnMouseEvent(double xpos, double ypos) = 0;
    virtual void OnScrollEvent(double xoffset, double yoffset) = 0;
    virtual void OnFrameUpdate() = 0;
};


#endif //MYOPENGL_INTWINDOWEVENTLISTENER_H

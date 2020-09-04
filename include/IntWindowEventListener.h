//
// Created by stagakis on 4/9/20.
//

#ifndef MYOPENGL_INTWINDOWEVENTLISTENER_H
#define MYOPENGL_INTWINDOWEVENTLISTENER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class IntWindowEventListener {
    public:
        virtual void OnKeyboardEvent(int key, int scancode, int action, int mods) {};
        virtual void OnMouseEvent(double xpos, double ypos) {};
        virtual void OnScrollEvent(double xoffset, double yoffset) {};
        virtual void OnFrameUpdate() {};
};


#endif //MYOPENGL_INTWINDOWEVENTLISTENER_H

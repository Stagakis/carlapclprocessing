#ifndef MYOPENGL_IWINDOWEVENTLISTENER_H
#define MYOPENGL_IWINDOWEVENTLISTENER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class IWindowEventListener {
    public:
        virtual void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {};
        virtual void OnMouseEvent(GLFWwindow* window, double xpos, double ypos) {};
        virtual void OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {};
        virtual void OnFrameUpdate(GLFWwindow* window, float deltaTime) {};
};


#endif //MYOPENGL_IWINDOWEVENTLISTENER_H

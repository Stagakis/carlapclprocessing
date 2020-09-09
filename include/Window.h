#ifndef MYOPENGL_WINDOW_H
#define MYOPENGL_WINDOW_H

#include "IntWindowEventListener.h"
#include <string>

class Window: public IntWindowEventListener{
public:
    Window(int width, int height, const std::string& windowName,bool fullscreen);
    GLFWwindow* GetGLFWwindowPtr();
    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;
private:
    GLFWwindow* window;
};


#endif //MYOPENGL_WINDOW_H

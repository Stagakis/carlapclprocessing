#ifndef MYOPENGL_WINDOWHANDLER_H
#define MYOPENGL_WINDOWHANDLER_H

#include "IWindowEventListener.h"
#include <string>

class WindowHandler: public IWindowEventListener{
public:
    WindowHandler(int width, int height, const std::string& windowName, bool fullscreen);
    GLFWwindow* GetGLFWwindowPtr();
    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;
private:
    GLFWwindow* window;
};


#endif //MYOPENGL_WINDOWHANDLER_H

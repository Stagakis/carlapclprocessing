#ifndef MYOPENGL_RENDERER_H
#define MYOPENGL_RENDERER_H

#include "IntWindowEventListener.h"
#include "Camera.h"

class Renderer : IntWindowEventListener {
    public:
        GLFWwindow* window;
        Camera camera;

        Renderer(GLFWwindow* _window, Camera& _camera);
        void MainLoop();
        void InitializeContext();
        void OnFrameUpdate(GLFWwindow* window, float deltaTime) override;
        void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;
        void OnMouseEvent(GLFWwindow* window, double xpos, double ypos) override;
        void OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset) override;
};


#endif //MYOPENGL_RENDERER_H

//
// Created by stagakis on 8/2/21.
//

#ifndef MYOPENGL_IMGUIMANAGER_H
#define MYOPENGL_IMGUIMANAGER_H


#include <memory>
#include <imgui.h>
class Application;

class ImguiManager {
public:
    static void InitializeImguiManager(Application& _app);
    static void DrawAllWindows();
    static void imGuiDrawWindow();
    static void imGuiOccupancyFactor();
    
private:
    static Application* app;
};


#endif //MYOPENGL_IMGUIMANAGER_H

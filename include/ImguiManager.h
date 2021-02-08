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
    ImguiManager(Application& app);

    void draw();
    void imGuiDrawWindow(ImVec4 &clear_color);
    void imGuiOccupancyFactor();

private:
    Application& _app;
};


#endif //MYOPENGL_IMGUIMANAGER_H

//
// Created by stagakis on 8/2/21.
//

#ifndef MYOPENGL_IMGUIMANAGER_H
#define MYOPENGL_IMGUIMANAGER_H

#include "Application.h"
#include <memory>
class ImguiManager {
public:
    explicit ImguiManager(Application *app);
    void newFrame();
    void endFrame();
    ~ImguiManager();
private:
    ImguiManager& operator=(const ImguiManager& other);
    const Application* _app;
};


#endif //MYOPENGL_IMGUIMANAGER_H

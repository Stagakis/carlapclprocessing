//
// Created by stagakis on 8/2/21.
//

#include "ImguiManager.h"

void ImguiManager::newFrame() {

}

ImguiManager::ImguiManager(Application *app) {
    _app = app;
}

ImguiManager::~ImguiManager() {
    delete _app;
}

void ImguiManager::endFrame() {

}

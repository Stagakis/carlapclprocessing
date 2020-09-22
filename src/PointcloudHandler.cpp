#include "PointcloudHandler.h"
#include <iostream>

PointcloudHandler::PointcloudHandler(std::vector<Pointcloud> & _pclList, int _index):pclList(_pclList),index(_index) {
}

void PointcloudHandler::OnKeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        index = std::min(pclList.size(), index + 1);
    }
    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        index = std::max((size_t)0, index-1);
    }
}

void PointcloudHandler::draw() {
    pclList[index].draw();
}

PointcloudHandler::PointcloudHandler() {

}

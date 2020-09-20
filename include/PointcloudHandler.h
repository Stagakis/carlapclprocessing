#ifndef MYOPENGL_POINTCLOUDHANDLER_H
#define MYOPENGL_POINTCLOUDHANDLER_H

#include "IWindowEventListener.h"
#include "Pointcloud.h"
class PointcloudHandler: public IWindowEventListener {
public:
    std::vector<Pointcloud> pclList;
    size_t index;
    explicit PointcloudHandler(std::vector<Pointcloud>& _pclList, int _index = 0);
    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;
    void draw();
};


#endif //MYOPENGL_POINTCLOUDHANDLER_H

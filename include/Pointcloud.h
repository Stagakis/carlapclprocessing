#ifndef MYOPENGL_POINTCLOUD_H
#define MYOPENGL_POINTCLOUD_H

#include <string>
#include <vector>
#include "Drawable.h"
#include "glm/glm.hpp"
struct Point{
    float x,y,z;
    Point(float _x, float _y, float _z){
        x = _x;
        y = _y;
        z = _z;
    }
    Point(){
        x=0;y=0;z=0;
    }
};

class Pointcloud : public Drawable{
    public:
        std::vector<Point> points;

        void draw() override;
        explicit Pointcloud(std::string filename);
};




#endif //MYOPENGL_POINTCLOUD_H

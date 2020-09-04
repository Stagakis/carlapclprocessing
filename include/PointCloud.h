//
// Created by stagakis on 21/8/20.
//

#ifndef MYOPENGL_POINTCLOUD_H
#define MYOPENGL_POINTCLOUD_H

#include <string>
#include <vector>
#include "Drawable.h"
struct Point{
    float x,y,z;
};

class PointCloud : Drawable{
    public:
        std::vector<Point> points;
        //unsigned int VBO, VAO;
        void draw() override;
        explicit PointCloud(std::string filename);
};




#endif //MYOPENGL_POINTCLOUD_H

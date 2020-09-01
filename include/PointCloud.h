//
// Created by stagakis on 21/8/20.
//

#ifndef MYOPENGL_POINTCLOUD_H
#define MYOPENGL_POINTCLOUD_H

#include <string>
#include <vector>
struct Point{
    float x,y,z;
};

class PointCloud {
    public:
        std::vector<Point> points;
        unsigned int VBO, VAO;
        void draw();
        explicit PointCloud(std::string filename);
};




#endif //MYOPENGL_POINTCLOUD_H

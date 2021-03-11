#ifndef MYOPENGL_POINTCLOUD_H
#define MYOPENGL_POINTCLOUD_H

#include <string>
#include <vector>
#include "Drawable.h"
#include "glm/glm.hpp"

static const glm::mat4 pcl_to_opengl_coord_system = glm::transpose(
    glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f)
);

static const glm::mat4 unreal_to_opengl_coord_system = glm::transpose(
    glm::mat4(
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f)
);


class Pointcloud : public Drawable{
    public:
        Pointcloud();
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> colors;
        void draw() override;
        explicit Pointcloud(std::string filename);
        Pointcloud(std::vector<glm::vec3> _points, std::vector<glm::vec3> _colors);
        void sendDataToGPU();
};




#endif //MYOPENGL_POINTCLOUD_H

#ifndef MYOPENGL_POINTCLOUD_H
#define MYOPENGL_POINTCLOUD_H

#include <string>
#include <vector>
#include "Drawable.h"
#include "glm/glm.hpp"


static const glm::mat4 imu_carla_to_opengl_coords = glm::transpose(
    glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f)
);

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
    glm::vec3 getVec() const{
        return glm::vec3(x,y,z);
    }
    void setVec(const glm::vec3 & pos){
        x = pos[0];
        y = pos[1];
        z = pos[2];
    }
    void setVec(const glm::vec4 & pos){
        x = pos[0];
        y = pos[1];
        z = pos[2];
    }
};

class Pointcloud : public Drawable{
    public:
        Pointcloud();
        std::vector<Point> points;
        std::vector<glm::vec3> colors;
        void draw() override;
        explicit Pointcloud(std::string filename);
        void sendDataToGPU();
};




#endif //MYOPENGL_POINTCLOUD_H

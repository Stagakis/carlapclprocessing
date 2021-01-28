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
        std::vector<Point> points;
        std::vector<glm::vec3> colors;
        void draw() override;
        explicit Pointcloud(std::string filename);
        void sendDataToGPU();
        void applyYaw(float angle);
};




#endif //MYOPENGL_POINTCLOUD_H

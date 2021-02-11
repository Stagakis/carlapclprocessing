#ifndef MYOPENGL_SERVER_H
#define MYOPENGL_SERVER_H

#include <Pointcloud.h>
#include "glm/glm.hpp"
#include "TransformParser.h"

struct obstacle{
    timing time;
    glm::vec3 center;
    std::vector<Point> points; //In world space coordinates
    obstacle(timing _time, glm::vec3 _center, std::vector<Point> _points):time(_time), center(_center), points(_points) {}
};

class Server {
public:
    static std::vector<obstacle> obstacles;
    static void addObstacle(std::vector<Point>& points, timing& tim);
    static std::vector<obstacle>& GetObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot);
public:
    //Singleton
    Server(Server const&) = delete;
    void operator=(Server const&)  = delete;
    Server() = delete;
private:

};


#endif //MYOPENGL_SERVER_H

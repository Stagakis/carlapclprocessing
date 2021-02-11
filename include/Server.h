#ifndef MYOPENGL_SERVER_H
#define MYOPENGL_SERVER_H

#include <Pointcloud.h>
#include "glm/glm.hpp"
#include "TransformParser.h"

struct obstacle{
    unsigned int timestamp;
    Point center;
    std::vector<Point> points; //In world space coordinates
};

class Server {
public:
    static std::vector<obstacle> obstacles;
    static void addObstacle(std::vector<Point>& points, timing tim);
    static std::vector<obstacle>& GetObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot);
public:
    //Singleton
    Server(Server const&) = delete;
    void operator=(Server const&)  = delete;
    Server() = delete;
private:

};


#endif //MYOPENGL_SERVER_H

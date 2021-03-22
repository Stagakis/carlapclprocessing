#ifndef MYOPENGL_SERVER_H
#define MYOPENGL_SERVER_H

#include <Pointcloud.h>
#include "glm/glm.hpp"
#include "TransformParser.h"

struct obstacle{
    timing time;
    glm::vec3 center;
    Pointcloud pcl; //In world space coordinates
    obstacle(timing _time, glm::vec3 _center, Pointcloud _pcl):time(_time), center(_center), pcl(_pcl) {}
};

class Server {
public:
    static std::vector<obstacle> obstacles;
    static void AddObstacle(Pointcloud pcl, timing& tim);
    static std::vector<Pointcloud *>  GetRelevantObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot);
public:
    //Singleton
    Server(Server const&) = delete;
    void operator=(Server const&)  = delete;
    Server() = delete;
private:

};


#endif //MYOPENGL_SERVER_H

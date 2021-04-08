#ifndef MYOPENGL_SERVER_H
#define MYOPENGL_SERVER_H
#include "helpers.h"
#include <Pointcloud.h>

#include <utility>
#include "glm/glm.hpp"
#include "Parsers/TransformParser.h"
#include <random>

static auto random_num_gen = std::default_random_engine();

struct obstacle{
    size_t id;
    timing time;
    glm::vec3 center;
    Pointcloud pcl; //In world space coordinates
    BoundingBox bb;
    obstacle(timing _time, glm::vec3 _center, Pointcloud _pcl, BoundingBox _bb):time(_time), center(_center), pcl(_pcl), bb(_bb) {
        id = random_num_gen();
    }
};

class Server {
public:
    static std::vector<obstacle> obstacles;
    static void AddObstacle(const Pointcloud& pcl, const timing& tim, const BoundingBox& bb);
    static std::vector<obstacle *>  GetRelevantObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot);
    static void ValidateObstacle(const obstacle& obs, const Pointcloud& pcl);
public:
    //Singleton
    Server(Server const&) = delete;
    void operator=(Server const&)  = delete;
    Server() = delete;
private:

};


#endif //MYOPENGL_SERVER_H

#include "Singletons/Server.h"

std::vector<obstacle> Server::obstacles = std::vector<obstacle>();


void Server::AddObstacle(const Pointcloud& pcl, const timing& tim, const BoundingBox& bb) {
    auto center = glm::vec3(0, 0, 0);

    auto obs = obstacle(tim, center, pcl, bb);

    obstacles.push_back(obs);
}

std::vector<obstacle *> Server::GetRelevantObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot) {
    std::vector<obstacle *> relevant_obstacles;
    for(auto& obstacle: obstacles){
        relevant_obstacles.push_back(&obstacle);
    }
    return relevant_obstacles;
}

void Server::ValidateObstacle(const obstacle &obs, const Pointcloud &pcl) {
    //Check if there is a need to update the database
}

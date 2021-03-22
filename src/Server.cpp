#include "Server.h"

std::vector<obstacle> Server::obstacles = std::vector<obstacle>();


void Server::AddObstacle(Pointcloud pcl, timing& tim) {
    auto center = glm::vec3(0, 0, 0);
    auto obs = obstacle(tim, center, pcl);
    obstacles.push_back(obs);
}

std::vector<Pointcloud *> Server::GetRelevantObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot) {
    std::vector<Pointcloud *> relevant_obstacles;
    for(auto& obstacle: obstacles){
        relevant_obstacles.push_back(&obstacle.pcl);
    }
    return relevant_obstacles;
}


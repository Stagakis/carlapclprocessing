#include "Server.h"

std::vector<obstacle> Server::obstacles = std::vector<obstacle>();


void Server::AddObstacle(Pointcloud pcl, timing& tim) {
    auto center = glm::vec3(0, 0, 0);
    auto obs = obstacle(tim, center, pcl);
    obstacles.push_back(obs);
}

void Server::DisplayObstacle(obstacle& obst, glm::vec3 vehicle_pos, glm::vec3 vehicle_rot) {
    //TODO do some checking whether it should be drawn or not based on pos and rot
    obst.pcl.draw();
}

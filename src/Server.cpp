#include "Server.h"

std::vector<obstacle> Server::obstacles = std::vector<obstacle>();


void Server::AddObstacle(std::vector<Point>& points, timing& tim) {
    auto center = glm::vec3(0, 0, 0);
    auto obs = obstacle(tim, center, points);
    obstacles.push_back(obs);
}

std::vector<obstacle> & Server::GetObstacles(glm::vec3 vehicle_pos, glm::vec3 vehicle_rot) {
    auto test = std::vector<obstacle>();
    return test;
}

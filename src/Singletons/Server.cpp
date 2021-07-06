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
    float threshold_factor = 1.15;
    for(auto& obstacle: obstacles){
        if(obstacle.id == obs.id){
            auto& bb_old = obstacle.bb;
            auto& bb_new = obs.bb;
            if( std::max(bb_old.area, bb_new.area) > threshold_factor * std::min(bb_old.area, bb_new.area)){
                std::cout << "Updating Database..." << std::endl;
                obstacle = obs;
            }
            else{
                std::cout << "No significant change detected..." << std::endl;
            }
            break;
        }
    }
}

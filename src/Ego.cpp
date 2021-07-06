#include "Ego.h"
#include "helpers.h"
#include <algorithm>
#include <execution>
#include "Server.h"


void Ego::checkForObstacles(int index, int threshold) {
    auto & pcl = data.get_pointcloud();

    static int last_index = -1;
    static int sleeping_index = 100;//it starts at 100 because it also detects random stuff in the beginning
    static size_t max_number_of_points = 0;

    if (index < sleeping_index || index == last_index) return;
    if (index > 200) return; //This is for video purpose TODO delete

    //Find bounding box of the blue area
    auto bb = calculateBoundingBox_color(pcl, glm::vec3(0,0,1));
    float shrink_factor = 0.1f;
    float x_len = (1 - shrink_factor) * std::abs(bb.max_x - bb.min_x);
    float z_len = 7; // std::min((1 - shrink_factor) * std::abs(max_z - min_z), 10.0f);
    glm::vec3 corner = glm::vec3(bb.min_x + shrink_factor * std::abs(bb.max_x - bb.min_x), 0 , bb.max_z); //max_z because we are looking at -z

    std::vector<size_t> indeces = std::vector<size_t>();

    for (int i = 0; i < pcl.points.size(); i++) {
        auto& p = pcl.points[i];
        auto& c = pcl.colors[i];

        if (p.x < corner.x || p.z > corner.z || p.x - corner.x > x_len || corner.z - p.z > z_len) {
            continue;
        }
        else {
            if (c == glm::vec3(1, 1, 0)) {
                indeces.push_back(i);
            }
        }
    }
    if (indeces.size() >= max_number_of_points)
        max_number_of_points = indeces.size();
    else{
        std::cout << "Sending hole..." << std::endl;
        std::vector<glm::vec3> points = std::vector<glm::vec3>(indeces.size());
        std::transform(std::execution::par_unseq, indeces.begin(), indeces.end(), points.begin(),
                       [&pcl](size_t index) -> glm::vec3{return pcl.points[index];});
        std::vector<glm::vec3> colors = std::vector<glm::vec3>(points.size(), glm::vec3(1,0,0));


        auto obst_pcl = Pointcloud(points, colors);
        obst_pcl.translation = pcl.translation;
        obst_pcl.ypr = pcl.ypr;
        obst_pcl.updateModelMatrix();

        auto obstacle_bb = calculateBoundingBox_color(obst_pcl, glm::vec3(1,0,0));

        Server::AddObstacle(obst_pcl, data.get_rgb_timing(), obstacle_bb);

        max_number_of_points = 0;
        sleeping_index = index + 500;
    }
    //std::cout << indeces.size() << std::endl;
    last_index = index;
}

Ego::Ego(const std::string& resources_folder):data(resources_folder) {}

void Ego::handleObstacle(const obstacle &obs) {
    std::cout << "Handling Obstacle" << std::endl;
    auto pcl =  data.get_pointcloud();

    glm::vec3 lidar_position = unreal_to_opengl_coord_system * glm::vec4(data.get_lidar_transformation().first, 1.0f);

    auto bb = obs.bb;

    glm::vec3 point1 = bb.translationMatrix * bb.rotationMatrix * glm::vec4(bb.min_x, bb.min_y, bb.min_z,1);
    glm::vec3 point2 = bb.translationMatrix * bb.rotationMatrix * glm::vec4(bb.max_x, bb.max_y, bb.max_z, 1);
    auto center = (point1 + point2)/2.0f;
    auto diameter = glm::length(point1 - point2);
    auto distance = glm::length(lidar_position - center);

    if(distance < diameter){
        std::cout << "Extracting Points inside the bounding box" << std::endl;
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> colors;
        //Extract the points in the bounding box
        for(int i = 0; i < pcl.points.size(); i++){
            auto& p = pcl.points[i];
            auto& c = pcl.colors[i];
            if(p.x > bb.min_x && p.x < bb.max_x &&
                p.y > bb.min_y && p.y < bb.max_y &&
                p.z > bb.min_z && p.z < bb.max_z)
            {
                points.push_back(p);
                colors.push_back(c);
            }
        }

        std::cout << "Sending to server for validation" << std::endl;
        auto obst_pcl = Pointcloud(points, colors);
        obst_pcl.translation = pcl.translation;
        obst_pcl.ypr = pcl.ypr;
        obst_pcl.updateModelMatrix();
        Server::ValidateObstacle(obs, obst_pcl);
    }

}

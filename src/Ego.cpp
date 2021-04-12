#include "Ego.h"
#include "helpers.h"
#include <algorithm>
#include <execution>
#include <future>
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp>
#include "Server.h"

size_t Ego::frameIndex=0;

void Ego::checkForObstacles(int index, int threshold) {
    auto & pcl = pointclouds[index];

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

        auto obstacle_bb = calculateBoundingBox_color(pcl, glm::vec3(1,0,0));

        auto obst_pcl = Pointcloud(points, colors);
        obst_pcl.translation = pcl.translation;
        obst_pcl.ypr = pcl.ypr;
        obst_pcl.updateModelMatrix();
        Server::AddObstacle(obst_pcl, transformData.rgbTiming[index], obstacle_bb);

        max_number_of_points = 0;
        sleeping_index = index + 500;
    }
    //std::cout << indeces.size() << std::endl;
    last_index = index;
}

Ego::Ego(std::string resources_folder) {
    //imu_data = ImuParser("../resources/imu.txt");
    /*
    std::ifstream inFile;
    inFile.open(resources_folder + "occupancy_ego1.csv");
    std::string line;
    for (; std::getline(inFile, line);){
        occupancyFactor.push_back(stof(line.substr(line.find(',') + 1, std::string::npos)));
    }
    */

    transformData = TransformParser(resources_folder + "camera_metadata.txt", resources_folder + "lidar_metadata.txt");
    steeringData = SteeringParser(resources_folder + "steering_true.txt");

#ifndef WINDOWS
    std::vector<std::string> files = glob(resources_folder + "*_saliency_segmentation.obj");
    std::vector<std::string> image_files = glob(resources_folder + "*.png");
#else
    std::vector<std::string> files = glob(resources_folder, ".obj");
    std::vector<std::string> image_files = glob(resources_folder, ".png");
#endif

    /*//
    std::vector<std::string> files_reduced;
    for(int i = 0 ; i < 150; i++){
        files_reduced.push_back(files[i]);
    }
    files = files_reduced;
    //*/

    std::vector<std::future<void>> futures;
    std::vector<ImageData> imgData(files.size() );
    std::cout << "Loading images........." << "\n";   //TODO VERY VERY UGLY PLEASE FIX IT
    const int batch_size = 50;
    for(int batch = 0; batch < files.size()/batch_size; batch++ ) {
        for (size_t i = batch_size*batch; i < batch_size*(batch + 1); i++) {
            futures.push_back(std::async(std::launch::async, loadTexture, &imgData, image_files[i], i));
        }
        for(int i = batch_size*batch; i < batch_size*(batch + 1); i++) {
            futures[i].get();
            images.emplace_back(imgData[i]);
        }
    }
    for(int i = (files.size()/batch_size) * batch_size; i < files.size(); i ++) { //Get the rest
        futures.push_back(std::async(std::launch::async, loadTexture, &imgData, image_files[i], i));
        futures[i].get();
        images.emplace_back(imgData[i]);
    }
    std::cout << "Finished loading images" << "\n";

    std::cout << "Loading OBJs..........." << "\n";
    pointclouds.resize(files.size());
    std::transform(std::execution::par_unseq, files.begin(), files.end(), pointclouds.begin(),
                   [](std::string file) -> Pointcloud { return Pointcloud(file); });
    std::cout << "Finished loading OBJs" << "\n";

    //PREPROCESSING rotate pointcloud based on steering and transform coordinates from Carla to Opengl system
    for(size_t i = 0 ; i < files.size() ; i++) {
        auto rot = glm::rotate(glm::radians(steeringData.angles[i]), glm::vec3(0, 0, 1));
        auto &points = pointclouds[i].points;
        for (int i = 0; i < points.size(); i++) {
            auto new_point = pcl_to_opengl_coord_system * rot * glm::vec4(points[i].x, points[i].y, points[i].z, 1.0f);
            points[i].x = new_point.x;
            points[i].y = new_point.y;
            points[i].z = new_point.z;
        }
        pointclouds[i].sendDataToGPU();
    }

    for(size_t i = 0 ; i < files.size(); i++){
        pointclouds[i].translation = glm::vec3(unreal_to_opengl_coord_system * glm::vec4(transformData.lidarPos[i], 1.0f));
        pointclouds[i].ypr = glm::vec3(-transformData.lidarRot[i][1], transformData.lidarRot[i][0], -transformData.lidarRot[i][2]);; // glm::vec3(-transformData.lidarRot[i][1], transformData.lidarRot[i][0], -transformData.lidarRot[i][2]); // roll is minus because we look at the -z axis
        pointclouds[i].updateModelMatrix();
    }
}

void Ego::handleObstacle(const obstacle &obs) {
    std::cout << "Handling Obstacle" << std::endl;
    auto pcl = pointclouds[frameIndex];
    glm::vec3 lidar_position = unreal_to_opengl_coord_system * glm::vec4(transformData.lidarPos[frameIndex] , 1.0f);

    auto bb = obs.bb;

    glm::vec3 point1 = pcl.model * pcl.rotationMatrix * glm::vec4(bb.min_x, bb.min_y, bb.min_z, 1);
    glm::vec3 point2 = pcl.model * pcl.rotationMatrix * glm::vec4(bb.max_x, bb.max_y, bb.max_z, 1);
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

Pointcloud &Ego::GetPointcloud() {    return pointclouds[frameIndex];}

ImageDrawable &Ego::GetImageDrawable() {    return images[frameIndex];}

float Ego::GetSteering() {    return steeringData.angles[frameIndex];}

std::pair<glm::vec3, glm::vec3> Ego::GetLidarTransform() {
    return std::pair<glm::vec3, glm::vec3>(transformData.lidarPos[frameIndex], transformData.lidarRot[frameIndex]);
}

std::pair<glm::vec3, glm::vec3> Ego::GetCameraTransform() {
    return std::pair<glm::vec3, glm::vec3>(transformData.rgbPos[frameIndex], transformData.rgbRot[frameIndex]);
}

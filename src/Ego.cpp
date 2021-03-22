#include "Ego.h"
#include "helpers.h"
#include <algorithm>
#include <execution>
#include <future>
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp>

void Ego::checkForObstacles(int index, int threshold) {
    static int last_index = -1;
    static int sleeping_index = 100;//it starts at 100 because it also detects random stuff in the beggining
    static size_t max_number_of_points = 0;

    if (index < sleeping_index || index == last_index) return;
    if (index > 200) return; //This is for video purpose TODO delete

    //Find bounding box of the blue area
    float min_x = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::min();
    float max_z = std::numeric_limits<float>::min();
    for (int i = 0; i < pointclouds[index].points.size(); i++) {
        auto& c = pointclouds[index].colors[i];
        if (c == glm::vec3(0, 0, 1)) { 
            auto& p = pointclouds[index].points[i];
            if (p.x > max_x) max_x = p.x;
            if (p.z > max_z) max_z = p.z;
            if (p.x < min_x) min_x = p.x;
            if (p.z < min_z) min_z = p.z;
        }   
    }

    float shrink_factor = 0.1f;
    float x_len = (1 - shrink_factor) * std::abs(max_x - min_x);
    float z_len = 7; // std::min((1 - shrink_factor) * std::abs(max_z - min_z), 10.0f);
    glm::vec3 corner = glm::vec3(min_x + shrink_factor * std::abs(max_x - min_x), 0 , max_z); //max_z because we are looking at -z

    std::vector<size_t> indeces = std::vector<size_t>();

    for (int i = 0; i < pointclouds[index].points.size(); i++) {
        auto& p = pointclouds[index].points[i];
        auto& c = pointclouds[index].colors[i];

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
        for (size_t i = 0; i < indeces.size() ; i++) {
            points[i] = pointclouds[index].points[indeces[i]];
        }
        std::vector<glm::vec3> colors = std::vector<glm::vec3>(points.size(), glm::vec3(1,0,0));
        auto obst_pcl = Pointcloud(points, colors);
        obst_pcl.translation = pointclouds[index].translation;
        obst_pcl.ypr = pointclouds[index].ypr;
        obst_pcl.updateModelMatrix();
        Server::AddObstacle(obst_pcl, transformData.rgbTiming[index]);

        max_number_of_points = 0;
        sleeping_index = index + 500;
    }
    std::cout << indeces.size() << std::endl;
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

#include "SensorData.h"
#include <future>
#include <algorithm>
#include <execution>
#include "helpers.h"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp>


size_t SensorData::frameIndex=0;

Pointcloud &SensorData::get_pointcloud() {    return pointclouds[frameIndex];}

ImageDrawable &SensorData::get_image() {    return images[frameIndex];}

float SensorData::get_steering() {    return steeringData.angles[frameIndex];}

std::pair<glm::vec3, glm::vec3> SensorData::get_lidar_transformation() {
    return std::pair<glm::vec3, glm::vec3>(transformData.lidarPos[frameIndex], transformData.lidarRot[frameIndex]);
}

std::pair<glm::vec3, glm::vec3> SensorData::get_camera_transformation() {
    return std::pair<glm::vec3, glm::vec3>(transformData.rgbPos[frameIndex], transformData.rgbRot[frameIndex]);
}

SensorData::SensorData(std::string resources_folder) {
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
    ///std::vector<std::string> files = glob(resources_folder + "*_saliency_segmentation.obj");
    std::vector<std::string> files = glob(resources_folder + "*.ply");
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

timing &SensorData::get_rgb_timing() {
    return transformData.rgbTiming[frameIndex];
}

timing &SensorData::get_lidar_timing() {
    return transformData.lidarTiming[frameIndex];
}


#include "Ego.h"
#include "helpers.h"
#include <algorithm>
#include <execution>
#include <future>

void Ego::checkForObstacles(int index, int threshold) {
    for (int i = 0; i < pointclouds[index].points.size(); i++) {
        auto& p = pointclouds[index].points[i];
        if (p.y < threshold) {
            std::cout << "hole found " << p.y << std::endl;
            break;
        }

    }
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

    //*//
    std::vector<std::string> files_reduced;
    for(int i = 0 ; i < files.size() - 300; i++){
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
                   [](std::string file) -> Pointcloud { return Pointcloud(std::move(file)); });
    std::cout << "Finished loading OBJs" << "\n";

    //PREPROCESSING done if the obj is rotated from the steering
    for(size_t i = 0 ; i < files.size() ; i++) {
        pointclouds[i].applyYaw(steeringData.angles[i]);
        pointclouds[i].sendDataToGPU();
    }

    for(size_t i = 0 ; i < files.size(); i++){
        pointclouds[i].translation = glm::vec3(imu_carla_to_opengl_coords * glm::vec4(transformData.lidarPos[i], 1.0f) );
        pointclouds[i].ypr = glm::vec3(-transformData.lidarRot[i][1], transformData.lidarRot[i][0], -transformData.lidarRot[i][2]); // roll is minus because we look at the -z axis
        pointclouds[i].updateModelMatrix();
    }

}

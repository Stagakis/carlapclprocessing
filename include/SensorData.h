#ifndef ARCARLA_SENSORDATA_H
#define ARCARLA_SENSORDATA_H

#include "Pointcloud.h"
#include "ImageDrawable.h"
#include "glm/vec3.hpp"
#include "ImuParser.h"
#include "TransformParser.h"
#include "SteeringParser.h"

class SensorData {
public:
    std::vector<Pointcloud> pointclouds;    //lidar

    Pointcloud& get_pointcloud();
    ImageDrawable& get_image();
    std::pair<glm::vec3, glm::vec3> get_lidar_transformation();
    std::pair<glm::vec3, glm::vec3> get_camera_transformation();
    timing& get_rgb_timing();
    timing& get_lidar_timing();
    float get_steering();

    explicit SensorData(std::string resources_folder);


    static size_t frameIndex;

private:
    std::vector<ImageDrawable> images;      //rgb camera
    ImuParser imu_data;                     //Deprecated use
    TransformParser transformData;
    SteeringParser steeringData;
};


#endif //ARCARLA_SENSORDATA_H

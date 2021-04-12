#ifndef MYOPENGL_EGO_H
#define MYOPENGL_EGO_H

#include <vector>
#include "Pointcloud.h"
#include "ImageDrawable.h"
#include "Parsers/ImuParser.h"
#include "Parsers/TransformParser.h"
#include "Parsers/SteeringParser.h"
#include "Server.h"


class Ego {
public:
    std::vector<Pointcloud> pointclouds;    //lidar

    Pointcloud& get_pointcloud();
    ImageDrawable& get_image();
    std::pair<glm::vec3, glm::vec3> get_lidar_transformation();
    std::pair<glm::vec3, glm::vec3> get_camera_transformation();
    float get_steering();

    void handleObstacle(const obstacle& obs);
    void checkForObstacles(int index, int threshold);
    explicit Ego(std::string resources_folder);

    static size_t frameIndex;

private:
    std::vector<ImageDrawable> images;      //rgb camera
    ImuParser imu_data;                     //Deprecated use
    TransformParser transformData;
    SteeringParser steeringData;
};


#endif //MYOPENGL_EGO_H

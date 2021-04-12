#ifndef MYOPENGL_EGO_H
#define MYOPENGL_EGO_H

#include <vector>
#include "Pointcloud.h"
#include "ImageDrawable.h"
#include "Parsers/ImuParser.h"
#include "Parsers/TransformParser.h"
#include "Parsers/SteeringParser.h"
#include "Server.h"
#include "SensorData.h"

class Ego {
public:
    //std::vector<Pointcloud> pointclouds;    //lidar

    SensorData data;

/*    Pointcloud& get_pointcloud();
    ImageDrawable& get_image();
    float get_steering();

    std::pair<glm::vec3, glm::vec3> get_lidar_transformation();
    std::pair<glm::vec3, glm::vec3> get_camera_transformation();
    static size_t frameIndex;*/

    void handleObstacle(const obstacle& obs);
    void checkForObstacles(int index, int threshold);
    explicit Ego(const std::string& resources_folder);



private:
/*    std::vector<ImageDrawable> images;      //rgb camera
    ImuParser imu_data;                     //Deprecated use
    TransformParser transformData;
    SteeringParser steeringData;*/
};


#endif //MYOPENGL_EGO_H

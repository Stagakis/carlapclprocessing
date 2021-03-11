#ifndef MYOPENGL_EGO_H
#define MYOPENGL_EGO_H

#include <vector>
#include "Pointcloud.h"
#include "ImageDrawable.h"
#include "ImuParser.h"
#include "TransformParser.h"
#include "SteeringParser.h"


class Ego {
public:
    std::vector<Pointcloud> pointclouds;    //lidar
    std::vector<ImageDrawable> images;      //rgb camera
    ImuParser imu_data;                     //Deprecated use
    TransformParser transformData;
    SteeringParser steeringData;
    void checkForObstacles(int index, int threshold);

    explicit Ego(std::string resources_folder);

};


#endif //MYOPENGL_EGO_H

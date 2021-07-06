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
    SensorData data;

    void handleObstacle(const obstacle& obs);
    void checkForObstacles(int index, int threshold);
    explicit Ego(const std::string& resources_folder);

private:

};


#endif //MYOPENGL_EGO_H

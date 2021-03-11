//
// Created by stagakis on 26/8/20.
//

#ifndef MYOPENGL_IMUPARSER_H
#define MYOPENGL_IMUPARSER_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
class ImuParser {

public:
    std::vector<int> frame;
    std::vector<float> timestamp;
    std::vector<glm::vec3> accel;
    std::vector<glm::vec3> gyro;
    std::vector<float> compass;

    explicit ImuParser(std::string filename );
    ImuParser();
};


#endif //MYOPENGL_IMUPARSER_H

#include "glm/glm.hpp"
#include <string>
#include <vector>

#ifndef MYOPENGL_TRANSFORMPARSER_H
#define MYOPENGL_TRANSFORMPARSER_H


class TransformParser {
public:
    std::vector<glm::vec3> rgbPos;
    std::vector<glm::vec3> lidarPos;
    std::vector<glm::vec3> rgbRot;
    std::vector<glm::vec3> lidarRot;

    explicit TransformParser(std::string filename);
};


#endif //MYOPENGL_TRANSFORMPARSER_H

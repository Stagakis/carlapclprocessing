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
    TransformParser(); //Deprecated
    void moveToOrigin(); //Not used
    explicit TransformParser(std::string filename); //Deprecated
    TransformParser(std::string rgb, std::string lidar);
private:
    void parseFile(const std::string filename, std::vector<glm::vec3>& outPos,std::vector<glm::vec3>& outRot);
};


#endif //MYOPENGL_TRANSFORMPARSER_H

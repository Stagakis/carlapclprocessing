#include "glm/glm.hpp"
#include <string>
#include <vector>

#ifndef MYOPENGL_TRANSFORMPARSER_H
#define MYOPENGL_TRANSFORMPARSER_H

struct timing{
    unsigned int frame;
    float timestamp;
    timing(int _frame, float _timestamp): frame(_frame), timestamp(_timestamp){ }
};

class TransformParser {
public:
    std::vector<glm::vec3> rgbPos;
    std::vector<glm::vec3> lidarPos;
    std::vector<glm::vec3> rgbRot;
    std::vector<glm::vec3> lidarRot;

    std::vector<timing> lidarTiming;
    std::vector<timing> rgbTiming;

    TransformParser(); //Deprecated
    void moveToOrigin(); //Not used
    explicit TransformParser(std::string filename); //Deprecated
    TransformParser(std::string rgb, std::string lidar);
private:
    void parseFile(const std::string filename, std::vector<glm::vec3>& outPos, std::vector<glm::vec3>& outRot, std::vector<timing> &outTiming);
};


#endif //MYOPENGL_TRANSFORMPARSER_H

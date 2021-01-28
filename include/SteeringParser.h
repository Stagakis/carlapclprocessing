#ifndef MYOPENGL_STEERINGPARSER_H
#define MYOPENGL_STEERINGPARSER_H

#include <string>
#include <vector>
#include <iostream>
class SteeringParser {
public:
    std::vector<float> angles;
    SteeringParser();
    explicit SteeringParser(std::string filename);
};


#endif //MYOPENGL_STEERINGPARSER_H

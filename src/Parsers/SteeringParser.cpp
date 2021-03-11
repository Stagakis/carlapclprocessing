//
// Created by stagakis on 28/1/21.
//

#include "Parsers/SteeringParser.h"
#include <fstream>
#include <sstream>

SteeringParser::SteeringParser() {

}

SteeringParser::SteeringParser(std::string filename) {
    std::ifstream myfile;
    myfile.open(filename);
    std::string line;
    for (; std::getline(myfile, line);)   //read stream line by line
    {
        angles.push_back(stof(line));
    }
}

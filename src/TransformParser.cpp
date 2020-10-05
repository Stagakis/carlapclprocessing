#include "TransformParser.h"
#include <iostream>
#include <fstream>
#include <sstream>

TransformParser::TransformParser(std::string filename) {
    std::ifstream myfile;
    myfile.open(filename);
    std::string line;
    std::vector<std::string> names = {"Transform(Location(", "Rotation(","Transform(Location(", "Rotation("};

    for (; std::getline(myfile, line);)   //read stream line by line
    {
        std::istringstream in(line);
        for(int i = 0; i<names.size() ; i++) {
            auto start = line.find(names[i]) + std::string(names[i]).length();
            auto length = line.substr(start, std::string::npos).find(')');

            std::string data_xyz = line.substr(start, length);

            float xyz[3];
            for(int j = 0; j < 3 ; j++){
                auto start2 = data_xyz.find('=') + 1;
                auto length2 = data_xyz.find(',') - start2;
                xyz[j] = stof(data_xyz.substr(start2, length2));
                data_xyz = data_xyz.substr(length2 + start2 + 2, std::string::npos);
            }

            if (i == 0) rgbPos.emplace_back(xyz[0], xyz[1], xyz[2]);
            if (i == 1) rgbRot.emplace_back(xyz[0], xyz[1], xyz[2]);
            if (i == 2) lidarPos.emplace_back(xyz[0], xyz[1], xyz[2]);
            if (i == 3) lidarRot.emplace_back(xyz[0], xyz[1], xyz[2]);

            line = line.substr(start + length + 1, std::string::npos);
        }

    }
}

TransformParser::TransformParser() {

}

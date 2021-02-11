#include "TransformParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

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

void TransformParser::moveToOrigin() {
    //*//
    std::vector<glm::vec3> rgbPos_new;
    std::vector<glm::vec3> lidarPos_new;
    std::vector<glm::vec3> rgbRot_new;
    std::vector<glm::vec3> lidarRot_new;

    for(int i = 0; i < rgbPos.size(); i++){
        rgbPos_new.push_back(rgbPos[i] - rgbPos[ std::max(i-1, 0)]);
        lidarPos_new.push_back(rgbRot[i] - rgbRot[ std::max(i-1, 0)]);
        rgbRot_new.push_back(lidarPos[i] - lidarPos[ std::max(i-1, 0)]);
        lidarRot_new.push_back(lidarRot[i] - lidarRot[ std::max(i-1, 0)]);
    }
    rgbPos = rgbPos_new;
    rgbRot = lidarPos_new;
    lidarPos = rgbRot_new;
    lidarRot = lidarRot_new;
    //*/

    /*//
    auto rgbPos_start = rgbPos[0];
    auto rgbRot_start = rgbRot[0];
    auto lidarPos_start = lidarPos[0];
    auto lidarRot_start = lidarRot[0];
    for(int i = 0; i < rgbPos.size(); i++){
        rgbPos[i] -= rgbPos_start;
        rgbRot[i] -= rgbRot_start;
        lidarPos[i] -= lidarPos_start;
        lidarRot[i] -= lidarRot_start;
    }
    //*/

}




TransformParser::TransformParser(std::string rgb, std::string lidar) {
    parseFile(rgb, rgbPos, rgbRot, rgbTiming);
    parseFile(lidar, lidarPos, lidarRot, lidarTiming);
}

void
TransformParser::parseFile(const std::string filename, std::vector<glm::vec3> &outPos, std::vector<glm::vec3> &outRot, std::vector<timing> &outTiming) {
    std::ifstream myfile;
    myfile.open(filename);
    std::string line;
    std::vector<std::string> names = {"frame=", "Transform(Location(", "Rotation("};

    for (; std::getline(myfile, line);)   //read stream line by line
    {
        std::istringstream in(line);
        for(int i = 0; i<names.size() ; i++) {
            if(names[i] == "frame="){
                auto start = line.find(names[i]) + std::string(names[i]).length();
                auto length = line.substr(start, std::string::npos).find(',');
                int frame = stoi(line.substr(start, length));

                start = line.find("timestamp=") + std::string("timestamp=").length();
                length = line.substr(start, std::string::npos).find(',');
                float timestamp = stof(line.substr(start, length));

                outTiming.emplace_back(frame, timestamp);
                continue;
            }

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

            if (i == 1) outPos.emplace_back(xyz[0], xyz[1], xyz[2]);
            if (i == 2) outRot.emplace_back(xyz[0], xyz[1], xyz[2]);

            line = line.substr(start + length + 1, std::string::npos);
        }

    }
}

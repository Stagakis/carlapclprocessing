//
// Created by stagakis on 26/8/20.
//

#include "CarlaImuParser.h"
#include <iostream>
#include <fstream>
#include <sstream>


CarlaImuParser::CarlaImuParser(std::string filename) {
    std::ifstream myfile;
    myfile.open(filename);
    std::string line;

    std::vector<std::string> names = {"frame=", "timestamp=",  "accelerometer=Vector3D(", "gyroscope=Vector3D(" , "compass="};
    for( ; std::getline(myfile, line); )   //read stream line by line
    {
        std::istringstream in(line);

        for(int i = 0; i<names.size() ; i++){

            if(names[i] == "accelerometer=Vector3D(" || names[i] == "gyroscope=Vector3D(") {
                auto start = line.find(names[i]) + std::string(names[i]).length();
                auto length = line.substr(start, -1).find(')');

                std::string data_xyz = line.substr(start, length);

                float xyz[3];
                for(int j = 0; j < 3 ; j++){
                    auto start = data_xyz.find('=') + 1;
                    auto length = data_xyz.find(',') - start;
                    data_xyz = data_xyz.substr(start, -1);
                    xyz[j] = stof(data_xyz);

                }
                if(names[i] == "accelerometer=Vector3D(") accel.push_back(glm::vec3(xyz[0], xyz[1], xyz[2]));
                if(names[i] == "gyroscope=Vector3D(") gyro.push_back(glm::vec3(xyz[0], xyz[1], xyz[2]));
                continue;
            }
            auto start = line.find(names[i]) + std::string(names[i]).length();
            auto length = line.find(',') - start;
            double value = stof(line.substr(start, length));
            if(names[i] == "frame=") frame.push_back(value);
            if(names[i] == "timestamp=") timestamp.push_back(value);
            if(names[i] == "compass=") compass.push_back(value);
        }

    }
    myfile.close();
}

CarlaImuParser::CarlaImuParser() {

}

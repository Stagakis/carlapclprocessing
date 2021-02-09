#include "Pointcloud.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp>

using namespace std;


Pointcloud::Pointcloud(std::string filename) {

    ifstream myfile;
    myfile.open(filename);
    string line;
    if(filename.substr(filename.size()-4, std::string::npos) == ".ply"){
        for(int i = 0 ; i < 7 ; i++){
            getline (myfile,line); //Ignore first seven lines
        }

        for( ; std::getline(myfile, line); )   //read stream line by line
        {
            std::istringstream in(line);
            Point new_point;
            in >> new_point.x >> new_point.y >> new_point.z;
            points.push_back(new_point);
            colors.emplace_back(1.0f, 1.0f, 1.0f);
        }

    }

    auto test = filename.substr(filename.size()-4, std::string::npos);
    if(filename.substr(filename.size()-4, std::string::npos) == ".obj") {
        for( ; std::getline(myfile, line); )   //read stream line by line
        {
            Point new_point;
            glm::vec3 new_color;
            line = line.substr(2, std::string::npos); //to remove the "v " at the start of each line
            std::istringstream in(line);
            in >> new_point.x >> new_point.y >> new_point.z >> new_color.r >> new_color.g >> new_color.b;

            points.push_back(new_point);
            colors.emplace_back(new_color.r, new_color.g, new_color.b);
        }
    }
    myfile.close();


}

void Pointcloud::draw() {
    glPointSize(5.0f);
    glBindVertexArray(VAO);

    glDrawArrays(GL_POINTS, 0, points.size());
}

void Pointcloud::applyYaw(float angle) {
    auto rot = glm::rotate(glm::radians(angle), glm::vec3(0,0,1));
    //std::cout<< "rot: " << glm::to_string(rot) << std::endl;
    for(int i = 0 ; i < points.size(); i ++){
        //std::cout<< "point before: " << glm::to_string(points[i].getVec());

        auto new_point = rot * glm::vec4(points[i].x,points[i].y,points[i].z,1.0f);
        points[i].x = new_point.x;
        points[i].y = new_point.y;
        points[i].z = new_point.z;
        //std::cout<< " After: " << glm::to_string(points[i].getVec()) << std::endl;
    }
}

void Pointcloud::sendDataToGPU() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_color);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(Point), &points[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(Point), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Pointcloud::Pointcloud() {

}


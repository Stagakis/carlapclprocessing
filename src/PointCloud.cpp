//
// Created by stagakis on 21/8/20.
//

#include "PointCloud.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>


using namespace std;


PointCloud::PointCloud(std::string filename) {

    ifstream myfile;
    myfile.open(filename);
    string line;

    for(int i = 0 ; i < 7 ; i++){
        getline (myfile,line); //Ignore first seven lines
    }

    for( ; std::getline(myfile, line); )   //read stream line by line
    {
        std::istringstream in(line);
        Point new_point;
        in >> new_point.x >> new_point.y >> new_point.z;
        points.push_back(new_point);
    }

    //cout<< *(float*)(&points[0]) << endl;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(Point), &points[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);


    myfile.close();
}

void PointCloud::draw() {
    glPointSize(5.0f);
    glBindVertexArray(VAO);

    glDrawArrays(GL_POINTS, 0, points.size());
}

#ifndef MYOPENGL_HELPERS_H
#define MYOPENGL_HELPERS_H

#include <vector>
#include <glob.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "iostream"
#include "Pointcloud.h"
#include "Application.h"
#define LOG(X) std::cout << X << std::endl;


void applyHole2Pointcloud(Pointcloud& pcl, Hole& hole){
    std::vector<Point> out_points;
    auto pcl_center = glm::vec3(pcl.model[3][0], pcl.model[3][1], pcl.model[3][2]);
    //LOG("pcl_center    : " << pcl_center.x << " " << pcl_center.y << " " << pcl_center.z);

    for (int i = 0; i < pcl.points.size(); i++) {
        glm::vec3 aPos(pcl.points[i].x, pcl.points[i].y,
                       pcl.points[i].z);
        glm::vec4 world_pos = pcl.model * glm::vec4(aPos, 1.0f);
        if (glm::distance(glm::vec3(world_pos), hole.center) <= hole.radius) {
            //LOG("Distance before: " << glm::distance(glm::vec3(world_pos), hole.center));
            std::cout << "BEFORE  " << glm::to_string(world_pos) << std::endl;
            world_pos.y -= hole.depth - (glm::distance(glm::vec3(world_pos), hole.center) / hole.radius) * hole.depth;
            aPos.z += 2 ;//hole.depth - (glm::distance(glm::vec3(world_pos), hole.center) / hole.radius) * hole.depth; //TODO fix the use of world_pos
            std::cout << "AFTER  " << glm::to_string(world_pos) << std::endl;

            glm::vec3 camera_to_point_ray = glm::vec3(world_pos) - pcl_center;
            float modifier = (-2.403600f - world_pos.y) / camera_to_point_ray.y;

            glm::vec3 intersection_point = glm::vec3(world_pos) + camera_to_point_ray * modifier;

            //LOG(pcl_center.x << " " << pcl_center.y << " " << pcl_center.z);
            //LOG("Intersection_point: " << intersection_point.x << " " << intersection_point.y << " " << intersection_point.z);
            //LOG(glm::distance(intersection_point, hole.center));
            if (distance(intersection_point, hole.center) > hole.radius) continue;
            //std::cout << "AFTER  " << glm::to_string(world_pos) << std::endl;
        }
        world_pos = glm::transpose(Carla_to_Opengl_coordinates) * world_pos;
        out_points.emplace_back(world_pos[0], world_pos[1], world_pos[2]);
    }
    pcl.points = std::vector<Point>(out_points);
}

void save2obj(const std::string filename, std::vector<Point> points){
    std::ofstream myfile;
    myfile.open(filename);
    for(int i = 0 ; i < points.size() ; i++){
        std::string line = "v " + std::to_string(points[i].x) + " " + std::to_string(points[i].y) + " " + std::to_string(points[i].z) + "\n";
        myfile.write(line.c_str(), line.size());
    }
    myfile.close();
}

std::vector<std::string> glob(const std::string &pattern) {
    using namespace std;

    // glob struct resides on the stack
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    // do the glob operation
    int return_value = ::glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    if(return_value != 0) {
        globfree(&glob_result);
        stringstream ss;
        ss << "glob() failed with return_value " << return_value << endl;
        throw std::runtime_error(ss.str());
    }

    // collect all the filenames into a std::list<std::string>
    vector<string> filenames;
    for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
        filenames.push_back(string(glob_result.gl_pathv[i]));
    }

    // cleanup
    globfree(&glob_result);

    // done
    return filenames;
}

GLFWwindow* createGlfwWindow(int width, int height, std::string name, bool fullscreen){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(width, height, name.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
}



#endif //MYOPENGL_HELPERS_H

#ifndef MYOPENGL_HELPERS_H
#define MYOPENGL_HELPERS_H

#include <vector>
#include <glob.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "iostream"
#include "Pointcloud.h"
#define LOG(X) std::cout << X << std::endl;

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

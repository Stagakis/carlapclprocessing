#ifndef MYOPENGL_HELPERS_H
#define MYOPENGL_HELPERS_H

#include <vector>
#ifndef WINDOWS
	#include <glob.h>
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "iostream"
#include "Pointcloud.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>

#define LOG(X) std::cout << X << std::endl

void saveImage(const char* filepath, GLFWwindow* w) ;


void saveFrame(int frameIndex, int numberLength, GLFWwindow* w);

void loadTexture(std::vector<ImageData>* imgData, const std::string filepath, int i);

void applyHole2Pointcloud(Pointcloud& pcl, Hole& hole);

void save2obj(const std::string filename, std::vector<Point> points);

std::vector<std::string> glob(const std::string &pattern);

GLFWwindow* createGlfwWindow(int width, int height, std::string name, bool fullscreen);


#endif //MYOPENGL_HELPERS_H

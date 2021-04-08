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
#include "ImageDrawable.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <fstream>


BoundingBox calculateBoundingBox_color(const Pointcloud& pcl, const glm::vec3& color);
BoundingBox calculateBoundingBox(const Pointcloud& pcl);

void saveImage(const char* filepath, GLFWwindow* w) ;

void saveFrame(int frameIndex, int numberLength, GLFWwindow* w);

void loadTexture(std::vector<ImageData>* imgData, const std::string filepath, int i);

//void applyHole2Pointcloud(Pointcloud& pcl, Hole& hole);

void save2obj(const std::string filename, std::vector<glm::vec3> points);

#ifndef WINDOWS
std::vector<std::string> glob(const std::string &pattern);
#else
std::vector<std::string> glob(const std::string& folder, const std::string& extension);
#endif

GLFWwindow* createGlfwWindow(int width, int height, std::string name, bool fullscreen);


#endif //MYOPENGL_HELPERS_H

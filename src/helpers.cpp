//
// Created by stagakis on 8/2/21.
//
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>
#include "helpers.h"
#include <filesystem>
namespace fs = std::filesystem;


BoundingBox calculateBoundingBox_color(const Pointcloud& pcl, const glm::vec3& color) {
    //Find bounding box of the blue area
    BoundingBox bb;
    bb.min_x = std::numeric_limits<float>::max();
    bb.min_y = std::numeric_limits<float>::max();
    bb.min_z = std::numeric_limits<float>::max();
    bb.max_x = std::numeric_limits<float>::min();
    bb.max_y = std::numeric_limits<float>::min();
    bb.max_z = std::numeric_limits<float>::min();
    for (int i = 0; i < pcl.points.size(); i++) {
        auto& c = pcl.colors[i];
        if (c == color) {
            auto& p = pcl.points[i];
            if (p.x > bb.max_x) bb.max_x = p.x;
            if (p.y > bb.max_y) bb.max_y = p.y;
            if (p.z > bb.max_z) bb.max_z = p.z;
            if (p.x < bb.min_x) bb.min_x = p.x;
            if (p.y < bb.min_y) bb.min_y = p.y;
            if (p.z < bb.min_z) bb.min_z = p.z;
        }
    }
    return bb;
}
BoundingBox calculateBoundingBox(const Pointcloud& pcl) {
    //Find bounding box of the blue area
    BoundingBox bb;
    bb.min_x = std::numeric_limits<float>::max();
    bb.min_y = std::numeric_limits<float>::max();
    bb.min_z = std::numeric_limits<float>::max();
    bb.max_x = std::numeric_limits<float>::min();
    bb.max_y = std::numeric_limits<float>::min();
    bb.max_z = std::numeric_limits<float>::min();
    for (int i = 0; i < pcl.points.size(); i++) {
        auto& p = pcl.points[i];
        if (p.x > bb.max_x) bb.max_x = p.x;
        if (p.y > bb.max_y) bb.max_y = p.y;
        if (p.z > bb.max_z) bb.max_z = p.z;
        if (p.x < bb.min_x) bb.min_x = p.x;
        if (p.y < bb.min_y) bb.min_y = p.y;
        if (p.z < bb.min_z) bb.min_z = p.z;
    }
    return bb;
}


void saveImage(const char* filepath, GLFWwindow* w){
    int width, height;
    glfwGetFramebufferSize(w, &width, &height);
    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;
    std::vector<char> buffer(bufferSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
}
void saveFrame(int frameIndex, int numberLength, GLFWwindow* w){
    //ffmpeg -framerate 2 -i frame_%04d.png -c:v libx264 -r 30 -pix_fmt yuv420p out.mp4
    std::string zeros;
    for(int i = 0; i< numberLength - std::to_string(frameIndex).size(); i++ ){
        zeros += "0";
    }
    saveImage(std::string("../recording/" + zeros + std::to_string(frameIndex) + ".png").c_str(), w);
}

void loadTexture(std::vector<ImageData>* imgData, const std::string filepath, int i){

    int width, height, nrChannels;
    imgData->operator[](i).data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    imgData->operator[](i).data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    imgData->operator[](i).width = width;
    imgData->operator[](i).height = height;
    imgData->operator[](i).nrChannels = nrChannels;
    imgData->operator[](i).name = std::string(filepath);
}

void save2obj(const std::string filename, std::vector<glm::vec3> points){
    std::ofstream myfile;
    myfile.open(filename);
    for(int i = 0 ; i < points.size() ; i++){
        std::string line = "v " + std::to_string(points[i].x) + " " + std::to_string(points[i].y) + " " + std::to_string(points[i].z) + "\n";
        myfile.write(line.c_str(), line.size());
    }
    myfile.close();
}

#ifndef WINDOWS
std::vector<std::string> glob(const std::string &pattern){
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
#else
std::vector<std::string> glob(const std::string& folder, const std::string& extension) {
    std::vector<std::string> file_paths;
    for (auto& p : fs::recursive_directory_iterator(folder))
    {
        if (p.path().extension() == extension) {
            file_paths.push_back(p.path().string());
        }
    }
    return file_paths;
}
#endif

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
    return window;
}

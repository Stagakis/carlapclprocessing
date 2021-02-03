#ifndef MYOPENGL_HELPERS_H
#define MYOPENGL_HELPERS_H
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <vector>
#include <glob.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "iostream"
#include "Pointcloud.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <stb_image_write.h>

#define LOG(X) std::cout << X << std::endl

void saveImage(const char* filepath, GLFWwindow* w) {
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
    saveImage(std::string("../recording/frame_" + zeros + std::to_string(frameIndex) + ".png").c_str(), w);
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

void applyHole2Pointcloud(Pointcloud& pcl, Hole& hole){
    float bed_level = hole.center.y + 0.1f;
    std::vector<Point> out_points;
    auto pcl_center = glm::vec3(pcl.model[3][0], pcl.model[3][1], pcl.model[3][2]);
    //LOG("pcl_center    : " << pcl_center.x << " " << pcl_center.y << " " << pcl_center.z);

    for (int i = 0; i < pcl.points.size(); i++) {
        glm::vec3 aPos(pcl.points[i].x, pcl.points[i].y,
                       pcl.points[i].z);
        glm::vec4 world_pos = pcl.model * Carla_to_Opengl_coordinates * glm::vec4(aPos, 1.0f);
        if (glm::distance(glm::vec3(world_pos), hole.center) <= hole.radius && world_pos.y <= bed_level) {
            //LOG("Distance before: " << glm::distance(glm::vec3(world_pos), hole.center));
            std::cout << "BEFORE  " << glm::to_string(world_pos) << std::endl;
            world_pos.y -= hole.depth - (glm::distance(glm::vec3(world_pos), hole.center) / hole.radius) * hole.depth;
            aPos.z += hole.depth - (glm::distance(glm::vec3(world_pos), hole.center) / hole.radius) * hole.depth; //TODO fix the use of world_pos
            std::cout << "AFTER  " << glm::to_string(world_pos) << std::endl;

            glm::vec3 camera_to_point_ray = glm::vec3(world_pos) - pcl_center;
            float modifier = (bed_level - world_pos.y) / camera_to_point_ray.y;

            glm::vec3 intersection_point = glm::vec3(world_pos) + camera_to_point_ray * modifier;

            if (distance(intersection_point, hole.center) > hole.radius) continue;

        }
        world_pos = glm::transpose(Carla_to_Opengl_coordinates) * world_pos;
        out_points.emplace_back(aPos[0], aPos[1], aPos[2]);
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
    return window;
}


#endif //MYOPENGL_HELPERS_H

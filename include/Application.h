#ifndef MYOPENGL_APPLICATION_H
#define MYOPENGL_APPLICATION_H
#define STB_IMAGE_IMPLEMENTATION
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glob.h> // glob(), globfree()
#include <string.h> // memset()
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <CarlaImuParser.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "IWindowEventListener.h"
#include "Pointcloud.h"
#include "ImageDrawable.h"

const unsigned int SCR_WIDTH =  1600; // 1920;//
const unsigned int SCR_HEIGHT = 800; // 1080;//
const std::string WINDOW_NAME = "CPSoS"; // 1080;//

struct Hole{
    glm::vec3 center{};
    float radius{}, depth{};
    Hole(glm::vec3 _center, float _radius, float _depth){center = _center; radius = _radius; depth = _depth;}
    Hole()= default;
};
//glm::mat4 Carla_to_Opengl_coordinates = glm::transpose(glm::mat4( 1.0));

glm::mat4 Carla_to_Opengl_coordinates = glm::transpose(glm::mat4(   1.0 , 0.0, 0.0, 0.0,
                                                                    0.0 , 0.0, -1.0, 0.0,
                                                                    0.0 , 1.0, 0.0, 0.0,
                                                                    0.0 , 0.0, 0.0, 1.0));

glm::mat4 imu_carla_to_opengl_coords = glm::transpose(glm::mat4(0.0f,1.0f,0.0f,0.0f,
                                                                0.0f,0.0f,1.0f,0.0f,
                                                                -1.0f,0.0f,0.0f,0.0f,
                                                                0.0f,0.0f,0.0f,1.0f));

class Application : public IWindowEventListener{
public:
    glm::mat4 world_to_lidar = glm::mat4(1.0f);
    glm::vec3 velocity = glm::vec3(0.0f);

    // settings

    Hole basic_hole;

    std::vector<Hole> holes;

    //std::vector<GLuint> textures;

    // camera
    Camera camera;
    GLFWwindow* window;
    //PointcloudHandler pclHand = PointcloudHandler();
    std::vector<ImageDrawable> images;
    std::vector<Pointcloud> pointclouds;
    CarlaImuParser imu_data;
    size_t frameIndex=0;

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    void imGuiDrawWindow(float& hole_radius, float& hole_depth, ImVec4 &clear_color);
    void setUpWindowEventHandlers();

    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;

    int AppMain();
};


#endif //MYOPENGL_APPLICATION_H

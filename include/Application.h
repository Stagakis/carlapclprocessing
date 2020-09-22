#ifndef MYOPENGL_APPLICATION_H
#define MYOPENGL_APPLICATION_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glob.h> // glob(), globfree()
#include <string.h> // memset()
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <CarlaImuParser.h>
#include "WindowHandler.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "IWindowEventListener.h"
#include "PointcloudHandler.h"

glm::mat4 Carla_to_Opengl_coordinates = glm::mat4(1.0f,0.0f,0.0f,0.0f,
                                                  0.0f,0.0f,1.0f,0.0f,
                                                  0.0f,-1.0f,0.0f,0.0f,
                                                  0.0f,0.0f,0.0f,1.0f);

glm::mat4 imu_carla_to_opengl_coords = glm::mat4(0.0f,1.0f,0.0f,0.0f,
                                                 0.0f,0.0f,1.0f,0.0f,
                                                 -1.0f,0.0f,0.0f,0.0f,
                                                 0.0f,0.0f,0.0f,1.0f);
// settings
struct Hole{
    glm::vec3 center;
    float radius, depth;
};

Hole basic_hole;
const unsigned int SCR_WIDTH =  1600; // 1920;//
const unsigned int SCR_HEIGHT = 900; // 1080;//

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLFWwindow* window;
WindowHandler* winHandler;
PointcloudHandler pclHand = PointcloudHandler();

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int pcl_index = 1;
glm::mat4 world_to_lidar(1.0f);
glm::vec3 velocity(0.0f);
CarlaImuParser imu_data;


class Application : public IWindowEventListener{
public:
    static bool initializeContext(const std::string& windowName,bool fullscreen);
    static void initializeImGui();
    static void imGuiDrawWindow(float& hole_radius, float& hole_depth, ImVec4 &clear_color);
    static void setUpWindowEventHandlers();
    static std::vector<std::string> glob(const std::string& pattern);


    static void HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;

    static int AppMain();

};


#endif //MYOPENGL_APPLICATION_H

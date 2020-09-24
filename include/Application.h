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
#include "PointcloudHandler.h"
#include "ImageDrawable.h"

const unsigned int SCR_WIDTH =  1600; // 1920;//
const unsigned int SCR_HEIGHT = 900; // 1080;//
const std::string WINDOW_NAME = "CPSoS"; // 1080;//

class Application : public IWindowEventListener{
public:

    glm::mat4 Carla_to_Opengl_coordinates = glm::mat4(1.0f,0.0f,0.0f,0.0f,
                                                      0.0f,0.0f,1.0f,0.0f,
                                                      0.0f,-1.0f,0.0f,0.0f,
                                                      0.0f,0.0f,0.0f,1.0f);

    glm::mat4 imu_carla_to_opengl_coords = glm::mat4(0.0f,1.0f,0.0f,0.0f,
                                                     0.0f,0.0f,1.0f,0.0f,
                                                     -1.0f,0.0f,0.0f,0.0f,
                                                     0.0f,0.0f,0.0f,1.0f);
    glm::mat4 world_to_lidar = glm::mat4(1.0f);
    glm::vec3 velocity = glm::vec3(0.0f);

    // settings
    struct Hole{
        glm::vec3 center;
        float radius, depth;
    };
    Hole basic_hole;

    std::vector<GLuint> textures;

    // camera
    Camera camera;
    GLFWwindow* window;
    PointcloudHandler pclHand = PointcloudHandler();
    std::vector<ImageDrawable> images;
    CarlaImuParser imu_data;

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    void imGuiDrawWindow(float& hole_radius, float& hole_depth, ImVec4 &clear_color);
    void setUpWindowEventHandlers();

    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;

    int AppMain();
};


#endif //MYOPENGL_APPLICATION_H

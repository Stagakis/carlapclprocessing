#ifndef MYOPENGL_APPLICATION_H
#define MYOPENGL_APPLICATION_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#ifndef WINDOWS
    #include <glob.h> // glob(), globfree()
#endif
#include <string.h> // memset()
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Parsers/ImuParser.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "IWindowEventListener.h"
#include "Pointcloud.h"
#include "ImageDrawable.h"
#include "Parsers/TransformParser.h"
#include <fstream>
#include "Parsers/SteeringParser.h"
#include "Singletons/ImguiManager.h"
#include "Ego.h"
#include "Singletons/Server.h"

const unsigned int SCR_WIDTH =  1600; // 1920;//
const unsigned int SCR_HEIGHT = 800; // 1080;//
const std::string WINDOW_NAME = "CPSoS"; // 1080;//

struct Hole{
    glm::vec3 center{};
    float radius{}, depth{};
    Hole(glm::vec3 _center, float _radius, float _depth){center = _center; radius = _radius; depth = _depth;}
    Hole()= default;
};

static struct SimulationStateVariables{
    size_t frameIndex=0;
}State;


class Application : public IWindowEventListener{
public:
    Camera camera;
    GLFWwindow* window;
    ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f); //clear color of window

    glm::vec3 cameraToLidarOffset;
    bool recording = false;     //If this is true, the simulation starts to run from the current frame (frameIndex)
                                //until the end and every frame is saved to disk


    // Ego vehicles and server
    std::vector<Ego> vehicles;
    int active_vehicle = 0;

    // timing
    size_t frameIndex = 0;
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    // postProcessing
    bool usePostprocessing = false;
    char iterNumber = 1;
    const char iterNumberStep = 1;

    //off-screen rendering
    unsigned int fbo;
    unsigned int fbTexture;

    void setUpWindowEventHandlers();
    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;
    void Initialization();

    int AppMain();
};


#endif //MYOPENGL_APPLICATION_H

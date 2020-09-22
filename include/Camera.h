

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <IWindowEventListener.h>
#include <iostream>
#include "Drawable.h"
// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  4.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  70.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera : public IWindowEventListener
{
public:
    // camera Attributes
    glm::vec3 Position{};
    glm::vec3 Front;
    glm::vec3 Up{};
    glm::vec3 Right{};
    glm::vec3 WorldUp{};
    bool cameraIsActive = true;
    bool following = false;

    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    void SetFollowingObject(Drawable* _obj);

    void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) override;

    void OnMouseEvent(GLFWwindow* window, double xpos, double ypos) override;

    void OnFrameUpdate(GLFWwindow* window, float deltaTime) override;

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

private:

    // calculates the front vector from the Camera's (updated) Euler Angles

    Drawable * obj = nullptr;
    void updateCameraVectors();
};
#endif


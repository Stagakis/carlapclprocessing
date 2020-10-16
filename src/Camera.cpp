#include <Camera.h>

Camera::Camera(glm::vec3 position, glm::vec3 up , float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    if(following){
        Yaw = obj->ypr[0];
        Pitch = obj->ypr[1];
        updateCameraVectors();
        Position = obj->translation + offset;
    }
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right, Front));

}

void Camera::OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cameraIsActive = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cameraIsActive = false;
        }
    }
}

void Camera::OnMouseEvent(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = xpos;
    static double lastY = ypos;

    if(cameraIsActive) {

        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }
}

void Camera::OnFrameUpdate(GLFWwindow* window, float deltaTime) {
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        MovementSpeed = 3*SPEED;
    else
        MovementSpeed = SPEED;
    float displacement = MovementSpeed * deltaTime;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Position += Front * displacement;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Position -= Right * displacement;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Position -= Front * displacement;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Position += Right * displacement;
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        Position += glm::vec3(0.0f, 1.0f, 0.0f) * displacement;
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        Position -= glm::vec3(0.0f, 1.0f, 0.0f) * displacement;
    }

}

void Camera::SetFollowingObject(Drawable* _obj, glm::vec3 _offset) {
    obj = _obj;
    offset = _offset;
}

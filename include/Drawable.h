#ifndef MYOPENGL_DRAWABLE_H
#define MYOPENGL_DRAWABLE_H
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stb_image.h>
#include <glm/gtx/euler_angles.hpp>

class Drawable {
    public:
        unsigned int VBO = 0, VAO = 0, EBO = 0, texture = 0;
        glm::mat4 model = glm::mat4(1.0f);
        void calculateModelMatrix(glm::vec3 translation, glm::vec3 pyr, glm::vec3 scale  = glm::vec3(1.0f, 1.0f, 1.0f));
        virtual void draw() = 0;
};

inline void Drawable::calculateModelMatrix(glm::vec3 translation, glm::vec3 pyr, glm::vec3 scale) {
    auto trans = glm::translate(glm::mat4(1.0f), translation);
    auto rot = glm::eulerAngleYXZ(glm::radians(pyr[1]), glm::radians(pyr[0]), glm::radians(pyr[2]));
    auto scl = glm::scale(glm::mat4(1.0f), scale);
    model = trans * rot * scl;
}


#endif //MYOPENGL_DRAWABLE_H

#ifndef MYOPENGL_DRAWABLE_H
#define MYOPENGL_DRAWABLE_H
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stb_image.h>
#include <glm/gtx/euler_angles.hpp>

class Drawable {
    public:
        unsigned int VBO = 0, VBO_color = 0, VAO = 0, EBO = 0, texture = 0;
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 translationMatrix;
        glm::mat4 rotationMatrix;

        glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 ypr = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        void updateModelMatrix(glm::vec3 rotationPivotOffest = glm::vec3(0.0f, 0.0f, 0.0f));
        virtual void draw() = 0;
};

inline void Drawable::updateModelMatrix(glm::vec3 rotationPivotOffest) {
    translationMatrix = glm::translate(glm::mat4(1.0f), translation);
    rotationMatrix = glm::eulerAngleYXZ(glm::radians(ypr[0]), glm::radians(ypr[1]), glm::radians(ypr[2]));

    //auto translationPivotFront =  glm::translate(glm::mat4(1.0f), +rotationPivotOffest);
    //auto translationPivotBack =  glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec4(-rotationPivotOffest, 1.0f) * rotationMatrix));

    auto scl = glm::scale(glm::mat4(1.0f), scale);
    model = translationMatrix * rotationMatrix * scl;
}


#endif //MYOPENGL_DRAWABLE_H

#ifndef MYOPENGL_DRAWABLE_H
#define MYOPENGL_DRAWABLE_H
#include <glm/glm.hpp>

class Drawable {
    public:
        unsigned int VBO = 0, VAO = 0, EBO = 0;
        glm::mat4 model = glm::mat4(1.0f);
        virtual void draw() = 0;
};


#endif //MYOPENGL_DRAWABLE_H

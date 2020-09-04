//
// Created by stagakis on 4/9/20.
//

#ifndef MYOPENGL_DRAWABLE_H
#define MYOPENGL_DRAWABLE_H


class Drawable {
    public:
        unsigned int VBO = 0, VAO = 0, EBO = 0;
        virtual void draw() = 0;
};


#endif //MYOPENGL_DRAWABLE_H

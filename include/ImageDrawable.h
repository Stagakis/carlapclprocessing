#ifndef MYOPENGL_IMAGEDRAWABLE_H
#define MYOPENGL_IMAGEDRAWABLE_H
#include "Drawable.h"
#include <string>


class ImageDrawable : public Drawable {
    explicit ImageDrawable(std::string filename);
    void draw() override;
};


#endif //MYOPENGL_IMAGEDRAWABLE_H

#ifndef MYOPENGL_IMAGEDRAWABLE_H
#define MYOPENGL_IMAGEDRAWABLE_H
#include "Drawable.h"
#include <string>


class ImageDrawable : public Drawable {
public:
    void draw() override;
    explicit ImageDrawable(std::string filename);
};


#endif //MYOPENGL_IMAGEDRAWABLE_H

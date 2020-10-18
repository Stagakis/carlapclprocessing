#ifndef MYOPENGL_IMAGEDRAWABLE_H
#define MYOPENGL_IMAGEDRAWABLE_H
#include "Drawable.h"
#include <string>

struct ImageData{
    unsigned char *data;
    int width;
    int height;
    int nrChannels;
};

class ImageDrawable : public Drawable {
public:
    void draw() override;
    void draw(GLuint texture);

    explicit ImageDrawable(std::string filename);
    ImageDrawable();
    ImageDrawable(unsigned char *data, int width, int height, int nrChannels);
    explicit ImageDrawable(ImageData& imgData);

};


#endif //MYOPENGL_IMAGEDRAWABLE_H

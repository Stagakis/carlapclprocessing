#ifndef MYOPENGL_SERVER_H
#define MYOPENGL_SERVER_H

#include <Pointcloud.h>
#include "Ego.h"


struct obstacle{
    unsigned int timestamp;
    Point center;
    std::vector<Point> points; //In world space coordinates
};

class Server {
public:
    std::vector<Ego>& vehicles;
    explicit Server(std::vector<Ego>& _vehicles);
};


#endif //MYOPENGL_SERVER_H

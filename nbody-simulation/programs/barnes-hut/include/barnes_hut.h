//
// Created by cjdcoy on 10/11/19.
//

#ifndef NBODY_PTHREAD_BARNES_HUT_H
#define NBODY_PTHREAD_BARNES_HUT_H

#include "body.h"
#include "x11-helpers.h"

#include <vector>


class Node {
public:
    Node(unsigned int depth)
    {
        _has_child = false;
        Depth = depth;
    }

    Node()
    {
        _has_child = false;
        Depth = 0;
    }
    void gen_childrens(XlibWrap &render);
    void gen_childrens();
    void init(std::vector<body_t> , float , float , float px = 0, float py = 0);
    void init(XlibWrap &render, std::vector<body_t> , float , float , float px = 0, float py = 0);
    void reset();

    std::vector<body_t> _bodies;
    std::vector<Node*> _child;
    bool _has_child;
    float _x_pos;
    float _y_pos;
    float _width;
    float _height;
    float _total_mass;
    float _center_mass_x;
    float _center_mass_y;
    unsigned int Depth;
private:
};

#endif //NBODY_PTHREAD_BARNES_HUT_H

//
// Created by cjdcoy on 10/11/19.
//

#include "barnes_hut.h"
#include <memory>

#define _MAX_DEPTH 10

/**
 * this functions adds new childrens to the curent node andadds them to the _child list of childrens
 * @param render for display purposes
 */
void Node::gen_childrens(XlibWrap &render)
{
    std::vector<body_t> NW_bodies, NE_bodies, SW_bodies, SE_bodies;

    for (unsigned int i = 0; i < _bodies.size(); i++) {
        if (_bodies[i].x_pos < (_x_pos + (_width / 2))) {
            if (_bodies[i].y_pos < (_y_pos + (_height / 2))) // NW
                NW_bodies.push_back(_bodies[i]);
            else //sw
                SW_bodies.push_back(_bodies[i]);
        }
        else {
            if (_bodies[i].y_pos < (_y_pos + (_height / 2))) //NE
                NE_bodies.push_back(_bodies[i]);
            else //SE
                SE_bodies.push_back(_bodies[i]);
        }
    }

    Node* NW = new Node(Depth + 1);
    Node* NE = new Node(Depth + 1);
    Node* SW = new Node(Depth + 1);
    Node* SE = new Node(Depth + 1);
    NW->init(render, NW_bodies, _width / 2, _height / 2, _x_pos, _y_pos);
    NE->init(render, NE_bodies, _width / 2, _height / 2, _x_pos + (_width / 2), _y_pos);
    SW->init(render, SW_bodies, _width / 2, _height / 2, _x_pos, _y_pos + (_height / 2));
    SE->init(render, SE_bodies, _width / 2, _height / 2, _x_pos + (_width / 2), _y_pos + (_height / 2));

    _child.push_back(new Node(Depth + 1));
    _child.push_back(new Node(Depth + 1));
    _child.push_back(new Node(Depth + 1));
    _child.push_back(new Node(Depth + 1));

    _has_child = true;
}
/**
 * this functions adds new childrens to the curent node andadds them to the _child list of childrens
 */
void Node::gen_childrens()
{
    std::vector<body_t> NW_bodies, NE_bodies, SW_bodies, SE_bodies;

    for (unsigned int i = 0; i < _bodies.size(); i++) {
        if (_bodies[i].x_pos < (_x_pos + (_width / 2))) {
            if (_bodies[i].y_pos < (_y_pos + (_height / 2)))
                NW_bodies.push_back(_bodies[i]);
            else //3
                SW_bodies.push_back(_bodies[i]);
        }
        else {
            if (_bodies[i].y_pos < (_y_pos + (_height / 2)))
                NE_bodies.push_back(_bodies[i]);
            else
                SE_bodies.push_back(_bodies[i]);
        }
    }

    Node* NW = new Node(Depth + 1);
    Node* NE = new Node(Depth + 1);
    Node* SW = new Node(Depth + 1);
    Node* SE = new Node(Depth + 1);

    NW->init( NW_bodies, _width / 2, _height / 2, _x_pos, _y_pos);
    NE->init(NE_bodies, _width / 2, _height / 2, _x_pos + (_width / 2), _y_pos);
    SW->init(SW_bodies, _width / 2, _height / 2, _x_pos, _y_pos + (_height / 2));
    SE->init(SE_bodies, _width / 2, _height / 2, _x_pos + (_width / 2), _y_pos + (_height / 2));

    _child.push_back(NW);
    _child.push_back(NE);
    _child.push_back(SW);
    _child.push_back(SE);
    _has_child = true;
}

/**
 *  this function assign the values of a node and then generates childrens depending on the actual depth
 * @param render for display
 * @param bodies list of bodies
 * @param width
 * @param height
 * @param px starting point x
 * @param py starting point y
 */
void Node::init(XlibWrap &render, std::vector<body_t> bodies, float width, float height, float px, float py)
{
    float mass = 0;
    double center_x = 0;
    double center_y = 0;
    unsigned int size = bodies.size();

    render.draw_square(px, py, px + width, py + height);
    if (size == 0)
        return;
    _bodies = bodies;
    _x_pos = px;
    _y_pos = py;
    _width = width;
    _height = height;
    for (unsigned int i = 0; i < size; i++) {
        mass += bodies[i].mass;
        center_x += bodies[i].x_pos;
        center_y += bodies[i].y_pos;
    }

    _total_mass = mass;
    _center_mass_x = center_x / size;
    _center_mass_y = center_y / size;
    if (_bodies.size() > 1 && Depth < _MAX_DEPTH)
        gen_childrens(render);
}

/**
 *  this function assign the values of a node and then generates childrens depending on the actual depth
 * @param bodies list of bodies
 * @param width
 * @param height
 * @param px starting point x
 * @param py starting point y
 */
void Node::init(std::vector<body_t> bodies, float width, float height, float px, float py)
{
    float mass = 0;
    double center_x = 0;
    double center_y = 0;
    unsigned int size = bodies.size();

    if (size == 0)
        return;
    _bodies = bodies;
    _x_pos = px;
    _y_pos = py;
    _width = width;
    _height = height;
    for (unsigned int i = 0; i < size; i++) {
        mass += bodies[i].mass;
        center_x += bodies[i].x_pos;
        center_y += bodies[i].y_pos;
    }

    _total_mass = mass;
    _center_mass_x = center_x / size;
    _center_mass_y = center_y / size;
    if (_bodies.size() > 1 && Depth < _MAX_DEPTH)
        gen_childrens();
}

void Node::reset()
{
    _bodies.clear();

    for (unsigned int i = 0; i < _child.size(); i++)
        _child[i]->reset();

    for (unsigned int i = 0; i < _child.size(); i++)
        delete _child[i];

    _child.clear();
    _has_child = false;
}

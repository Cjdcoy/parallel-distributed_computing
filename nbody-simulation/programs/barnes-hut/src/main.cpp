#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <fstream>

#include "body.h"
#include "barnes_hut.h"
#include "timer.h"
#include "x11-helpers.h"

void show_array(std::vector<double> &arr) {
    for (const auto &value : arr) {
        std::cout << value << " ,";
    }
    std::cout << std::endl;
}
static void print_user()
{
    std::cout << "Name: Fabodyen Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 3, nbody, barnes hut implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./nbody_bh NB_ITER GRAPHICAL" << std::endl;
    std::cout << "          NB_ITER: number of iterations" << std::endl;
    std::cout << "          GRAPHICAL: 0 for grapihics + quad tree, 1 graphics, 2 without graphics" << std::endl;
    std::cout << "EXAMPLE:  ./bodyn/nbody_bh 1000 0\n" << std::endl;
    exit(0);
}


std::vector<body_t> g_bodies;
Node g_nodes;
double const g_node_thresh = 0.5;             //Threshold for node calculations
float g_soft = 10;


/**
 *  generate the g_bodies
 * @param nb_body number of g_bodies tog enerate
 */
void generate_bodies(std::vector<body_t> &bodies, int nb_body) {
    body_t body;
    std::random_device dev;
    std::mt19937 rng(SEED);

    std::uniform_real_distribution<double> pos_dist(POS_LOW_BOUNDS, WIN_SIZE);
    std::uniform_real_distribution<double> vel_dist(VEL_LOW_BOUNDS, VEL_HIGH_BOUNDS);
    std::uniform_real_distribution<double> mass_dist(VEL_LOW_BOUNDS, VEL_HIGH_BOUNDS);

    for (size_t x = 0 ; x < nb_body ; x++) {
        body.x_pos = pos_dist(rng);
        body.y_pos = pos_dist(rng);
        body.x_vel = vel_dist(rng);
        body.y_vel = vel_dist(rng);
        body.x_accel = ACCEL_DEFAULT;
        body.y_accel = ACCEL_DEFAULT;
        body.mass = 1;
        bodies.emplace_back(body);
    }
}

/**
 *
 * @param results contains numbers to write
 *
 * writes the performances into a file for a python graph interpretation
 */
void write_results(std::tuple<std::string, int, int64_t> &result)
{
    std::string output;
    std::ofstream myfile;

    output = std::get<0>(result) + " " + to_string(std::get<1>(result)) + " " + to_string(std::get<2>(result)) + "\n";
    myfile.open ("nbody.out", std::ios_base::app);
    myfile << output;
    myfile.close();
}

/**
 * render pixels
 * @param render xlib wrapper
 * @param nb_body number of body to display
 */
static void rendering(XlibWrap &render, int nb_body)
{
    int x, y;

    for (unsigned int it = 0; it < nb_body; it += 1) {
        x = g_bodies[it].x_pos;
        y = g_bodies[it].y_pos;
        if (x >= 0 && x <= WIN_SIZE && y >= 0 && y <= WIN_SIZE)
            render.put_pixel(x, y, 255, 0, 0);
    }
    render.put_image(0, 0, 0, 0, WIN_SIZE, WIN_SIZE);
    render.flush();
    render.clear();
}

/**
 * compute the acceleration
 * @param node
 * @param body
 */
void compute_node_accel(Node* node, body_t* body)
{
    double x_vector = node->_center_mass_x - body->x_pos;
    double y_vector = node->_center_mass_y - body->y_pos;
    double sqrt_dist = x_vector * x_vector + y_vector * y_vector + g_soft * g_soft;
    double dist = sqrt_dist * sqrt_dist * sqrt_dist;
    double inv_dist_cube = 1.0f / (sqrt(dist));
    double accel = (node->_total_mass * inv_dist_cube * G_CONST);

    body->x_accel += x_vector * accel;
    body->y_accel += y_vector * accel;
}

/**
 * check each node of the three and compute the new accelerations for each body present within the nodes
 * @param node node to check
 * @param body body to check
 */
void check_node(Node* node, body_t* body)
{
    double x_diff = (node->_center_mass_x - body->x_pos);
    double y_diff = (node->_center_mass_y - body->y_pos);
    double dist_sqrt = ((x_diff) * (x_diff) + (y_diff) * (y_diff));
    double width_sqrt = node->_width * node->_width;

    if (width_sqrt / dist_sqrt < g_node_thresh || node->_has_child == false)
        compute_node_accel(node, body);
    else {
        if (node->_child[0]->_bodies.size() > 0)
            check_node(node->_child[0], body);
        if (node->_child[1]->_bodies.size() > 0)
            check_node(node->_child[1], body);
        if (node->_child[2]->_bodies.size() > 0)
            check_node(node->_child[2], body);
        if (node->_child[3]->_bodies.size() > 0)
            check_node(node->_child[3], body);
    }
}

void compute_tree()
{
    for (unsigned int i = 0; i < g_bodies.size(); i++)
        check_node(&g_nodes, &g_bodies[i]);
}

/**
 * update the bodies position depending on their velocity.
 * This also allow bodies to bound on screen limitations and on other bodies.
 */
void update_bodies()
{
    for (unsigned int i = 0; i < g_bodies.size(); i++) {
        if ((g_bodies[i].x_pos >= WIN_SIZE && g_bodies[i].x_vel >= 0) || (g_bodies[i].x_pos <= 1 && g_bodies[i].x_vel <= 0))
            g_bodies[i].x_vel = -g_bodies[i].x_vel;

        if ((g_bodies[i].y_pos >= WIN_SIZE && g_bodies[i].y_vel >= 0) || (g_bodies[i].y_pos <= 1 && g_bodies[i].y_vel <= 0))
            g_bodies[i].y_vel = -g_bodies[i].y_vel;

        g_bodies[i].x_vel += g_bodies[i].x_accel / g_bodies[i].mass;
        g_bodies[i].y_vel += g_bodies[i].y_accel / g_bodies[i].mass;
        g_bodies[i].x_pos += g_bodies[i].x_vel;
        g_bodies[i].y_pos += g_bodies[i].y_vel;
    }
}

/**
 * main function that handle the display and that update the g_bodies depending on the number of iterations
 * @param iteration number of iterations to perform
 * @param nb_body number of g_bodies
 * @param nb_thread number of threads
 */
static void run_graphic(int iteration, int nb_body, int mode)
{
    std::tuple<std::string, int, int64_t> result;
    XlibWrap render;
    Timer t;
    long long end;

    render.init();
    t.start("test");
    std::cout << "There's a usleep of 20000 in this implementation loop so you can see the quad tree updating" << std::endl;
    for (unsigned int i_iteration = 0; i_iteration < iteration; i_iteration += 1) {
        update_bodies();
        rendering(render, nb_body);
        g_nodes.reset();
        if (mode == 0)
            g_nodes.init(render, g_bodies, WIN_SIZE, WIN_SIZE);
        else
            g_nodes.init(g_bodies, WIN_SIZE, WIN_SIZE);
        compute_tree();
        usleep(20000);
    }
    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("bh ", iteration, end);
    write_results(result);
    render.destroy();
}

/**
 * main function that handle the display and that update the g_bodies depending on the number of iterations
 * @param iteration number of iterations to perform
 * @param nb_body number of g_bodies
 * @param nb_thread number of threads
 */
static void run(int iteration)
{
    std::tuple<std::string, int, int64_t> result;
    Timer t;
    long long end;

    t.start("test");
    for (unsigned int i_iteration = 0; i_iteration < iteration; i_iteration += 1) {
        update_bodies();
        g_nodes.reset();
        g_nodes.init(g_bodies, WIN_SIZE, WIN_SIZE);
        compute_tree();
    }
    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("bh", iteration, end);
    write_results(result);
}

int main(int ac, char **av) {
    if (ac != 3) {
        print_usage();
        return 1;
    }

    int nb_body = NB_BODIES;
    int nb_iter = std::atoi(av[1]);
    int graphic = std::atoi(av[2]);

    print_user();
    generate_bodies(g_bodies, nb_body);
    if (graphic <= 1)
        run_graphic(nb_iter, nb_body, graphic);
    else
        run(nb_iter);
}

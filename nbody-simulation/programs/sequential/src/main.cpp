#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <fstream>

#include "timer.h"
#include "body.h"
#include "x11-helpers.h"

#define G_CONST 0.01
#define POS_LOW_BOUNDS 0
#define VEL_LOW_BOUNDS 0
#define VEL_HIGH_BOUNDS 1
#define MASS_LOW_BOUNDS 0
#define MASS_HIGH_BOUNDS 10
#define ACCEL_DEFAULT 0

void show_array(std::vector<double> &arr) {
    for (const auto &value : arr) {
        std::cout << value << " ,";
    }
    std::cout << std::endl;
}


static void print_user()
{
    std::cout << "Name: Fabien Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 3, nbody, sequential implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./nbody_openmp NB_ITER GRAPHICAL" << std::endl;
    std::cout << "          NB_ITER: number of iterations" << std::endl;
    std::cout << "          GRAPHICAL: 0 graphics, 1 without graphics" << std::endl;
    std::cout << "EXAMPLE:  ./bin/nbody_openmp 1000 0\n" << std::endl;
    exit(0);
}


std::vector<body_t> g_bodies;

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
 * update the g_bodies acceleration
 * @param it position of the obdy in the vector of g_bodies
 * @param nb_body total number of g_bodies
 */
void update_acceleration(unsigned int it, int nb_body) {
    double dx, dy, tmp;

    g_bodies[it].x_accel= 0;
    g_bodies[it].y_accel = 0;
    for (unsigned int i = 0; i < nb_body; i += 1) {
        if (i != it) {
            dx = g_bodies[it].x_pos - g_bodies[i].x_pos;
            dy = g_bodies[it].y_pos - g_bodies[i].y_pos;
            tmp = G_CONST * g_bodies[i].mass / pow(sqrt(dx*dx+dy*dy), 3);
            g_bodies[it].x_accel += tmp * (g_bodies[i].x_pos - g_bodies[it].x_pos);
            g_bodies[it].y_accel += tmp * (g_bodies[i].y_pos - g_bodies[it].y_pos);
        }
    }
}

/**
 * update the g_bodies velocity
 * @param it position of the obdy in the vector of g_bodies
 */
void update_velocity(unsigned int it) {
    g_bodies[it].x_vel += g_bodies[it].x_accel * 1;
    g_bodies[it].y_vel += g_bodies[it].y_accel * 1;
}



/**
 * update the g_bodies position
 * @param it position of the obdy in the vector of g_bodies
 */
void update_position(unsigned int it) {
    if ((g_bodies[it].x_pos >= WIN_SIZE && g_bodies[it].x_vel >= 0) || (g_bodies[it].x_pos <= 1 && g_bodies[it].x_vel <= 0))
        g_bodies[it].x_vel = -g_bodies[it].x_vel;

    if ((g_bodies[it].y_pos >= WIN_SIZE && g_bodies[it].y_vel >= 0) || (g_bodies[it].y_pos <= 1 && g_bodies[it].y_vel <= 0))
        g_bodies[it].y_vel = -g_bodies[it].y_vel;

    g_bodies[it].x_vel += g_bodies[it].x_accel / g_bodies[it].mass;
    g_bodies[it].y_vel += g_bodies[it].y_accel / g_bodies[it].mass;
    g_bodies[it].x_pos += g_bodies[it].x_vel;
    g_bodies[it].y_pos += g_bodies[it].y_vel;
}

/**
 * comppute and update all the g_bodies positions dependeing on their velocity and acceleration
 * @param nb_body number of body
 * @param nb_thread number of thread
 */
void update_physics(int nb_body) {
    for (unsigned int it = 0; it < nb_body; it += 1) {
        update_acceleration(it, nb_body);
        update_velocity(it);
        update_position(it);
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
 * main function that handle the display and that update the g_bodies depending on the number of iterations
 * @param iteration number of iterations to perform
 * @param nb_body number of g_bodies
 * @param nb_thread number of threads
 */
static void run_graphic(int iteration, int nb_body)
{
    std::tuple<std::string, int, int64_t> result;
    XlibWrap render;
    Timer t;
    int x, y;
    long long end;

    render.init();
    t.start("test");
    for (unsigned int i_iteration = 0; i_iteration < iteration; i_iteration += 1) {
        update_physics(nb_body);
        rendering(render, nb_body);
    }
    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("openmp ", 1, end);
    write_results(result);
    render.destroy();
}

/**
 * main function that handle the display and that update the g_bodies depending on the number of iterations
 * @param iteration number of iterations to perform
 * @param nb_body number of g_bodies
 * @param nb_thread number of threads
 */
static void run(int iteration, int nb_body)
{
    std::tuple<std::string, int, int64_t> result;
    Timer t;
    int x, y;
    long long end;

    t.start("test");
    for (unsigned int i_iteration = 0; i_iteration < iteration; i_iteration += 1) {
        update_physics(nb_body);
    }
    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("openmp ", 1, end);
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
    if (graphic == 0)
        run_graphic(nb_iter, nb_body);
    else
        run(nb_iter, nb_body);
}

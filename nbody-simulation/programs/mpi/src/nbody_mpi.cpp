
#include <cmath>
#include <map>
#include <iostream>
#include <random>
#include <algorithm>

#include "nbody_mpi.h"
#include "timer.h"

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

std::vector<body_t> g_bodies;

/**
 *
 * @param content content to print
 * @param size size of the array
 * @param n print until n
 *
 *
 */
static void print_data(int *content, int size, int n)
{
    for (int i = 0 ; i < size && i < n ; i++)
        std::cout << content[i] << " ";
    std::cout<<std::endl;
}

/**
 * render pixels
 * @param render xlib wrapper
 * @param nb_body number of body to display
 */
static void rendering(XlibWrap &render)
{
    int x, y;

    for (unsigned int it = 0; it < NB_BODIES; it += 1) {
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
 *  generate the g_bodies
 * @param nb_body number of g_bodies tog enerate
 */
void populate_g_bodies(int nb_body) {
    body_t body;
    std::random_device dev;
    std::mt19937 rng(dev());

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
        g_bodies.emplace_back(body);
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
void update_physics(int nb_body, int start, int end) {
    for (unsigned int it = start; it < end; it += 1) {
        update_acceleration(it, nb_body);
        update_velocity(it);
        update_position(it);
    }
}

/**
 * serialize
 * @param to_serialize data to serialize
 * @return
 */
std::vector<double> serialize(std::vector<body_t> &to_serialize)
{
    std::vector<double> serialized;

    for (auto &body : to_serialize) {
        serialized.emplace_back(body.x_pos);
        serialized.emplace_back(body.y_pos);
        serialized.emplace_back(body.x_vel);
        serialized.emplace_back(body.y_vel);
        serialized.emplace_back(body.x_accel);
        serialized.emplace_back(body.y_accel);
        serialized.emplace_back(body.mass);
    }
    return serialized;
}

/**
 *
 * @param to_unserialize data to unserialize
 * @return
 */
std::vector<body_t> unserialize(std::vector<double> &to_unserialize)
{
    std::vector<body_t> nodes;

    for (int x = 0 ; x < to_unserialize.size() ; x+=7) {
        body_t node;

        node.x_pos =    to_unserialize[x];
        node.y_pos =    to_unserialize[x + 1];
        node.x_vel =    to_unserialize[x + 2];
        node.y_vel =    to_unserialize[x + 3];
        node.x_accel =  to_unserialize[x + 4];
        node.y_accel =  to_unserialize[x + 5];
        node.mass =     to_unserialize[x + 6];

        nodes.emplace_back(node);
    }
    return nodes;
}

/**
 * partitioning
 * @param nb_threads number of processes
 * @return shares
 */
static std::vector<std::pair<int, int>> partitioning(int nb_threads)
{
    if (nb_threads > NB_BODIES / 2)
        nb_threads = NB_BODIES / 2 - 1;

    int portion = NB_BODIES / nb_threads;
    int added = 0;
    std::vector<std::pair<int, int>> shares;
    std::pair<int, int> share;

    for (int thread = 0 ; thread < nb_threads -1 ; thread++) {
        share = make_pair(added, added + portion);
        shares.emplace_back(share);
        added += portion;
    }
    share = make_pair(added, NB_BODIES);
    shares.emplace_back(share);
    added += NB_BODIES - added;
    return shares;
}

/**
 *
 * @param shares
 * @param mpi_tot_id
 */
void master_receive_body_parts(std::vector<std::pair<int, int>> &shares, int mpi_tot_id)
{
    for (int i = 1; i < mpi_tot_id ; i++)
        MPI_Recv(&g_bodies[shares[i].first], (shares[i].second - shares[i].first) * 7, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

/**
 *
 * @param random_vect vector containing all the random numbers unsorted
 * @param vect_size size of the vector
 * @param mpi_tot_id total number of nodes
 *
 * this is basically the master main function which whill scatter, process and gather the vector of random numbers
 */
void mpi_master(XlibWrap &xlib, int nb_iteration)
{
    int mpi_tot_id; MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    std::vector<double> b_serialized;
    std::vector<std::pair<int, int>> shares = partitioning(mpi_tot_id);

    populate_g_bodies(NB_BODIES);
    for (unsigned int i_iteration = 0; i_iteration < nb_iteration; i_iteration += 1) {
        b_serialized = serialize(g_bodies);
        //send_data
        MPI_Bcast(&b_serialized[0], NB_BODIES * 7, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        //compute
        update_physics(NB_BODIES, shares[0].first, shares[0].second);
        //receive
        master_receive_body_parts(shares, mpi_tot_id);
        //display
        rendering(xlib);
    }
}

/**
 *
 * @param random_vect vector containing all the random numbers unsorted
 * @param vect_size size of the vector
 * @param mpi_tot_id total number of nodes
 *
 * this is basically the master main function which whill scatter, process and gather the vector of random numbers
 */
void mpi_master(int nb_iteration)
{
    int mpi_tot_id; MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    std::vector<double> b_serialized;
    std::vector<std::pair<int, int>> shares = partitioning(mpi_tot_id);

    populate_g_bodies(NB_BODIES);
    for (unsigned int i_iteration = 0; i_iteration < nb_iteration; i_iteration += 1) {
        //serialize
        b_serialized = serialize(g_bodies);
        //send
        MPI_Bcast(&b_serialized[0], NB_BODIES * 7, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        //compute
        update_physics(NB_BODIES, shares[0].first, shares[0].second);
        //receive
        master_receive_body_parts(shares, mpi_tot_id);
    }
}

/**
 *
 * @param vect_size size of the random vector that has been generated by the master
 * @param mpi_tot_id total number of nodes
 *
 *  his is basically the master main function which whill receive, process and send its portion of the vector of random numbers
 */
void mpi_slave(int mpi_id, int nb_iteration)
{
    int mpi_tot_id; MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    std::vector<int> map;
    std::vector<double> b_serialized;
    std::vector<std::pair<int, int>> shares = partitioning(mpi_tot_id);
    int size_to_send = shares[mpi_id].second - shares[mpi_id].first;

    b_serialized.resize(NB_BODIES* 7);
    for (unsigned int i_iteration = 0; i_iteration < nb_iteration; i_iteration += 1) {
        //receive
        MPI_Bcast(&b_serialized[0], NB_BODIES * 7, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        //unserialize
        g_bodies = unserialize(b_serialized);
        //compute
        update_physics(NB_BODIES, shares[mpi_id].first, shares[mpi_id].second);
        //send
        MPI_Ssend(&g_bodies[shares[mpi_id].first], size_to_send * 7, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}
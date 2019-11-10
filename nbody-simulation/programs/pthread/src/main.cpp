#include <vector>
#include <iostream>
#include <thread>
#include <tuple>
#include <map>
#include <iostream>
#include <fstream>

#include "body.h"
#include "timer.h"
#include "x11-helpers.h"

#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>
class Barrier
{

public:
    Barrier(int count)
            : thread_count(count)
            , counter(0)
            , waiting(0)
    {}

    void wait()
    {
        //fence mechanism
        std::unique_lock<std::mutex> lk(m);
        ++counter;
        ++waiting;
        cv.wait(lk, [&]{return counter >= thread_count;});
        cv.notify_one();
        --waiting;
        if (waiting == 0)
        {
            //reset barrier
            counter = 0;
        }
        lk.unlock();
    }

    void finish()
    {
        thread_count-=1;
    }
private:
    std::mutex m;
    std::condition_variable cv;
    int counter;
    int waiting;
    std::atomic<int> thread_count;
};


void show_array(std::vector<double> &arr) {
    for (const auto &value : arr) {
        std::cout << value << " ,";
    }
    std::cout << std::endl;
}

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

static void print_user()
{
    std::cout << "Name: Fabien Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 3, nbody, pthread implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./nbody_pthread NB_ITER NB_THREADS GRAPHICAL" << std::endl;
    std::cout << "          NB_ITER: number of iterations" << std::endl;
    std::cout << "          NB_THREADS: number of trheads" << std::endl;
    std::cout << "          GRAPHICAL: 0 for grapihics, 1 without graphics" << std::endl;
    std::cout << "EXAMPLE:  ./bin/nbody_pthread 1000 12 1\n" << std::endl;
    exit(0);
}


std::vector<body_t> g_bodies;

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

static std::vector<std::pair<int, int>> partitioning(int nb_body, int nb_threads)
{
    if (nb_threads > nb_body / 2)
        nb_threads = nb_body / 2 - 1;

    int portion = nb_body / nb_threads;
    int added = 0;
    std::vector<std::pair<int, int>> shares;
    std::pair<int, int> share;

    for (int thread = 0 ; thread < nb_threads -1 ; thread++) {
        share = make_pair(added, added + portion);
        shares.emplace_back(share);
        added += portion;
    }
    share = make_pair(added, nb_body);
    shares.emplace_back(share);
    added += nb_body - added;
    return shares;
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

void thread_loop_graphic(Barrier &b, int nb_body, int start, int end, int it, int thread)
{
    XlibWrap render;

    if (thread == 0)
        render.init();
    for (unsigned int i_iteration = 0; i_iteration < it; i_iteration += 1) {
        update_physics(nb_body, start, end);
        b.wait();
        if (thread == 0)
            rendering(render, nb_body);
    }
    b.finish();
    if (thread == 0)
        render.destroy();
}

void thread_loop(Barrier &b, int nb_body, int start, int end, int it, int thread)
{
    for (unsigned int i_iteration = 0; i_iteration < it; i_iteration += 1) {
        update_physics(nb_body, start, end);
        b.wait();
    }
    b.finish();
}
/**
 * main function that handle the display and that update the g_bodies depending on the number of iterations
 * @param iteration number of iterations to perform
 * @param nb_body number of g_bodies
 * @param nb_thread number of threads
 */
static void run_graphic(int iteration, int nb_body, int nb_threads)
{
    std::vector<std::thread> threads;
    Barrier barrier(nb_threads);
    std::vector<std::pair<int, int>> shares;
    std::tuple<std::string, int, int64_t> result;
    Timer t;
    long long end;

    shares = partitioning(nb_body, nb_threads);
    t.start("test");
    for (int thread = 0 ; thread < nb_threads ; thread++)
        threads.emplace_back(std::thread(thread_loop_graphic, std::ref(barrier), nb_body, shares[thread].first, shares[thread].second, iteration, thread));
    for (auto &thread : threads)
        thread.join();

    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("pthread", nb_threads, end);
    write_results(result);
}

/**
 * main function that handle the display and that update the g_bodies depending on the number of iterations
 * @param iteration number of iterations to perform
 * @param nb_body number of g_bodies
 * @param nb_thread number of threads
 */
static void run(int iteration, int nb_body, int nb_threads)
{
    std::vector<std::thread> threads;
    Barrier barrier(nb_threads);
    std::vector<std::pair<int, int>> shares;
    std::tuple<std::string, int, int64_t> result;
    Timer t;
    long long end;

    shares = partitioning(nb_body, nb_threads);
    t.start("test");
    for (int thread = 0 ; thread < nb_threads ; thread++)
        threads.emplace_back(std::thread(thread_loop, std::ref(barrier), nb_body, shares[thread].first, shares[thread].second, iteration, thread));
    for (auto &thread : threads)
        thread.join();

    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("pthread", nb_threads, end);
    write_results(result);
}

int main(int ac, char **av) {
    if (ac != 4) {
        print_usage();
        return 1;
    }
    Timer t;

    int nb_body = NB_BODIES;
    int nb_iter = std::atoi(av[1]);
    int nb_thread = std::atoi(av[2]);
    int graphic = std::atoi(av[3]);
    long long end;
    print_user();
    generate_bodies(g_bodies, nb_body);
    if (graphic == 0)
        run_graphic(nb_iter, nb_body, nb_thread);
    else
        run(nb_iter, nb_body, nb_thread);
}

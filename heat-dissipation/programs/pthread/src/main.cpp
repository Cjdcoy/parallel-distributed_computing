#include <vector>
#include <iostream>
#include <thread>
#include <tuple>
#include <map>
#include <iostream>
#include <fstream>

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


static void print_user()
{
    std::cout << "Name: Fabien Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 4, heat, pthread implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./heat_pthread NB_ITER NB_THREAD GRAPHICAL" << std::endl;
    std::cout << "          NB_ITER: number of iterations" << std::endl;
    std::cout << "          NB_THREAD: number of threads" << std::endl;
    std::cout << "          GRAPHICAL: 0 graphics, 1 without graphics" << std::endl;
    std::cout << "EXAMPLE:  ./bin/heat_pthread 1000 12 0\n" << std::endl;
    exit(0);
}

/**
 * render pixels
 * @param render xlib wrapper
 * @param nb_body number of body to display
 */
static void rendering(XlibWrap &render)
{
    render.put_image(0, 0, 0, 0, WIN_SIZE, WIN_SIZE);
    render.flush();
    render.clear();
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
    myfile.open ("heat.out", std::ios_base::app);
    myfile << output;
    myfile.close();
}

void init_map(std::vector<std::vector<double>> &map)
{
    for (int i = 1; i < WIN_SIZE - 1; i++ ) {
        map[i][0] = 20.0;
        map[0][i] = 20.0;
        map[i][WIN_SIZE-1] = 20.0;
        map[WIN_SIZE-1][i] = 20.0;
    }
    for (int i_row = 1; i_row < 190; i_row += 1) {
        for (int i_width = (WIN_SIZE/2)-40; i_width < (WIN_SIZE/2)+40; i_width += 1) {
            map[i_row][i_width] = 100.0;
        }
    }
}

void parallel_heat(XlibWrap &render, Barrier &b, std::vector<std::vector<double>> &map, std::vector<std::vector<double>> &tmp, int nb_iter, int start, int end, int graphic, int thread)
{
    for (int iteration = 0; iteration <  nb_iter; iteration++) {
        for (int i = start; i < end && i < WIN_SIZE - 1; i++) {
            for (int j = 0; j < WIN_SIZE; j++)
                tmp[i][j] = map[i][j];
        }
        for (int i = start; i < end && i < WIN_SIZE - 1; i++) {
            for (int j = 1; j < WIN_SIZE - 1; j++)
                map[i][j] = (tmp[i - 1][j] + tmp[i + 1][j] + tmp[i][j - 1] + tmp[i][j + 1]) * 0.25;
        }
        b.wait();
        if (graphic == 0 && thread == 0) {
            int g = 0; int b = 0; int r = 0; double n;
            for (int i = 0; i < WIN_SIZE - 1; i++) {
                for (int j = 0; j < WIN_SIZE; j++) {
                    n = map[i][j] / 100;
                    b = 255 * (1 - n);
                    r = 255 - b;
                    render.put_pixel(j, i, r, g, b);
                }
            }
            rendering(render);
        }
    }
    b.finish();
}

static std::vector<std::pair<int, int>> partitioning(int win_size, int nb_threads)
{
    if (nb_threads > win_size / 2)
        nb_threads = win_size / 2 - 1;

    int portion = win_size / nb_threads;
    int added = 1;
    std::vector<std::pair<int, int>> shares;
    std::pair<int, int> share;

    for (int thread = 0 ; thread < nb_threads -1 ; thread++) {
        share = make_pair(added, added + portion);
        shares.emplace_back(share);
        added += portion;
    }
    share = make_pair(added, win_size);
    shares.emplace_back(share);
    return shares;
}

void compute_heat(std::vector<std::vector<double>> &map, XlibWrap &render, int nb_iter, int nb_thread, int graphic)
{
    std::vector<std::vector<double>> tmp(WIN_SIZE);
    std::vector<std::thread> threads;
    std::vector<std::pair<int, int>> shares = partitioning(WIN_SIZE, nb_thread);
    Barrier b(nb_thread);

    for (auto &row : tmp) row.resize(WIN_SIZE);

    for (int thread = 0 ; thread < nb_thread ; thread++) {
        threads.push_back(std::thread(parallel_heat, std::ref(render), std::ref(b), std::ref(map), std::ref(tmp), nb_iter, shares[thread].first, shares[thread].second, graphic, thread));
    }
    for (auto &thread : threads)
        thread.join();
}

void run(int nb_iter, int nb_thread, int graphic)
{
    std::vector<std::vector<double>> map(WIN_SIZE);
    std::tuple<std::string, int, int64_t> result;
    XlibWrap render;
    Timer t;
    long long end;

    t.start("global");

    for (auto &row : map) row.resize(WIN_SIZE);

    if (graphic == 0)
        render.init();

    init_map(map);
    t.start("timer");
    compute_heat(map, render, nb_iter, nb_thread, graphic);
    end = t.get_elapsed_time("timer", "oui", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("pthread", nb_thread, end);

    write_results(result);

    end = t.get_elapsed_time("global", "oui", true);
    std::cout << "global runTime is " << end / (float)1000000 << " seconds" << std::endl;
    if (graphic == 0)
        render.destroy();
}


int main(int ac, char **av) {
    if (ac != 4) {
        print_usage();
        return 1;
    }
    Timer t;

    int nb_iter = std::atoi(av[1]);
    int nb_thread = std::atoi(av[2]);
    int graphic = std::atoi(av[3]);

    print_user();
    run(nb_iter, nb_thread, graphic);
    std::cout << std::endl;
}
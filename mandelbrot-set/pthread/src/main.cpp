
#include <utility>
#include <cmath>
#include <thread>
#include <map>
#include <utility>
#include <fstream>

#include "timer.h"
#include "main.h"
#include "x11-helpers.h"

int g_max_iter = 2000;
#define WIN_SIZE 200

typedef struct complextype
{
    float real, imag;
} Compl;

/**
 *
 * @param xlib
 * @param lines
 * @param x_conditions
 */
void parallel_mandlebrot(XlibWrap &xlib, std::vector<int> &lines, std::pair<int, int> &x_conditions)
{
    Compl   z, c;
    int k = 0;
    float ratio1 = WIN_SIZE / (float)2;
    float ratio2 = WIN_SIZE / (float)4;

    for (auto y : lines) {
        for (int x = x_conditions.first ; x < WIN_SIZE ; x+= x_conditions.second) {
            z.real = z.imag = 0.0;
            c.real = ((float) x - ratio1)/(ratio2);               // scale factors for 800 x 800 window
            c.imag = ((float) y - ratio1)/(ratio2);
            k = 0;
            double  lengthsq = 0, temp = 0;
            while (lengthsq < 20 && k < g_max_iter) {                                     //     iterate for pixel color

                temp = z.real*z.real - z.imag*z.imag + c.real;
                z.imag = 2.0*z.real*z.imag + c.imag;
                z.real = temp;
                lengthsq = z.real*z.real+z.imag*z.imag;
                if (k == g_max_iter && k > 0)
                    xlib.put_pixel(x, y, -(std::sqrt(z.real / (float) g_max_iter) * 155), 0, 120);
                k++;
            }
            //this is purely for optimization so we only put pixels when necessary
            if (k != g_max_iter && k > 0)
                xlib.put_pixel(x, y, -(std::sqrt(z.real / (float) g_max_iter) * 155), 0, 120);
        }
    }
}

static std::map<int, std::vector<int>> partitioning_per_line(std::vector<std::pair<int, int>> &x_conditions, int nb_threads)
{
    std::map<int, std::vector<int>> shares;
    int thread = 0;

    for (int i = 0 ; i < nb_threads; i++)
        x_conditions.emplace_back(make_pair(0, 1));

    for (int idx = 0 ; idx < WIN_SIZE ; idx++) {
        shares[thread].push_back(idx);
        if (thread + 1 == nb_threads)
            thread = 0;
        else
            thread++;
    }
    return shares;
}

static std::map<int, std::vector<int>> partioning_per_pixel(std::vector<std::pair<int, int>> &x_conditions, int nb_threads)
{

    std::map<int, std::vector<int>> shares;

    for (int i = 0 ; i < nb_threads; i++)
        x_conditions.emplace_back(make_pair(i, nb_threads));
    for (int idx = 0 ; idx < WIN_SIZE; idx++) {
        for (int thread = 0 ; thread < nb_threads; thread++)
            shares[thread].push_back(idx);
    }
    return shares;
}

static std::map<int, std::vector<int>> partioning_per_zone(std::vector<std::pair<int, int>> &x_conditions, int nb_threads)
{

    std::map<int, std::vector<int>> shares;
    int thread = 0;
    int share = WIN_SIZE / nb_threads;

    for (int i = 0 ; i < nb_threads; i++)
        x_conditions.emplace_back(make_pair(0, 1));
    for (int idx = 0 ; idx < WIN_SIZE ; idx+=share) {
        for (int start = idx ; start < (idx + share) ; start++) {
            shares[thread].push_back(start);
        }
        thread++;
    }
    return shares;
}

void scheduler(XlibWrap &xlib, int nb_threads, int mode)
{
    std::vector<std::thread> threads;
    std::vector<std::pair<int, int>> x_conditions;
    std::map<int, std::vector<int>> shares;

    if (mode == 0)
        shares = partitioning_per_line(x_conditions, nb_threads);
    else if (mode == 1)
        shares = partioning_per_pixel(x_conditions, nb_threads);
    else
        shares = partioning_per_zone(x_conditions, nb_threads);
    for (int thread = 0 ; thread < nb_threads ; thread++) {
        threads.push_back(std::thread(parallel_mandlebrot, std::ref(xlib), std::ref(shares[thread]), std::ref(x_conditions[thread])));
    }
    for (auto &thread : threads)
        thread.join();

}

static void print_user()
{
    std::cout << "Name: Fabien Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 2, mandlebrot, Pthread implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./mandlebrot_pthread NB_ITER MODE NB_THREADS" << std::endl;
    std::cout << "          NB_ITER: number of iterations for the mandlebrot " << std::endl;
    std::cout << "          MODE: -1: all modes, 0: line partitioning, 1: pixel partitioning, 2: zone partitioning" << std::endl;
    std::cout << "          NB_THREAD: number of threads that will be used" << std::endl;
    std::cout << "EXAMPLE:  ./bin/mandlebrot_pthread 1000 0 12\n" << std::endl;
    exit(0);
}

/**
 *
 * @param results contains numbers to write
 *
 * writes the performances into a file for a python graph interpretation
 */
void write_results(std::vector<std::tuple<std::string, int, int64_t>> &results)
{
    std::string output;
    std::ofstream myfile;

    for (auto &result : results) {
        output += std::get<0>(result) + " " + to_string(std::get<1>(result)) + " " + to_string(std::get<2>(result)) + "\n";
    }
    myfile.open ("mandlebrot.out", std::ios_base::app);
    myfile << output;
    myfile.close();
}

void bench(XlibWrap &xlib, int nb_threads, int mode)
{
    Timer t;
    std::vector<std::tuple<std::string, int, int64_t>> results;
    char mode_names[3][10] = {"line", "pixel", "zone"};
    int actual_mode = mode == -1 ? mode + 1 : mode;
    long long end;

    for (; actual_mode < 3 ; actual_mode++) {
        t.start("time");
        scheduler(xlib, nb_threads, actual_mode);
        end = t.get_elapsed_time("time", "end", true);
        std::cout << "runTime is " << end / (float)1000000 << " seconds (" << mode_names[actual_mode] << ")" << std::endl;
        results.emplace_back(make_tuple(mode_names[actual_mode], nb_threads, end));
        if (mode != -1)
            break;
    }
    //write_results(results);
}

int main(int ac, char **av) {
    if (ac < 4)
        print_usage();
    XlibWrap xlib;

    g_max_iter = atoi(av[1]);
    int mode = atoi(av[2]);
    int nb_threads = atoi(av[3]);

    if (g_max_iter < 0 || (mode < -1 || mode > 2) || nb_threads < 1) {
        std::cout << "ARGUMENT ERROR, nb_iter or mode invalid." << std::endl;
        print_usage(); return 1;
    }
    print_user();
    // Function calling
    xlib.init(WIN_SIZE);
    bench(xlib, nb_threads, mode);
    //without this sleep the pixels don't have the time to show up if the computation's too fast
    usleep(500000);
    xlib.put_image(0, 0, 0, 0, WIN_SIZE, WIN_SIZE);
    xlib.flush();
    sleep(10);
    xlib.destroy();

    return 0;
}
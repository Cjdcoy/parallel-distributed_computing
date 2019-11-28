#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <fstream>

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
    std::cout << "Name: Fabien Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 4, heat, sequential implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./heat_seq NB_ITER GRAPHICAL" << std::endl;
    std::cout << "          NB_ITER: number of iterations" << std::endl;
    std::cout << "          GRAPHICAL: 0 graphics, 1 without graphics" << std::endl;
    std::cout << "EXAMPLE:  ./bin/heat_seq 1000 0\n" << std::endl;
    exit(0);
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

void init_map(std::vector<std::vector<double>> &map)
{
    for (int i = 1; i < WIN_SIZE - 1; i++ ) {
        map[i][0] = 20.0;
        map[0][i] = 20.0;
        map[i][WIN_SIZE-1] = 20.0;
        map[WIN_SIZE-1][i] = 20.0;
    }

    for (int i_row = 1; i_row < 20; i_row += 1) {
        for (int i_width = (WIN_SIZE/2)-40; i_width < (WIN_SIZE/2)+40; i_width += 1) {
            map[i_row][i_width] = 100.0;
        }
    }
}

void compute_heat(std::vector<std::vector<double>> &map, XlibWrap &render, int nb_iter, int graphic)
{
    std::vector<std::vector<double>> tmp(WIN_SIZE);

    for (auto &row : tmp) row.resize(WIN_SIZE);

    for (int iteration = 0; iteration <  nb_iter; iteration++) {
        for (int i = 0; i < WIN_SIZE; i++) {
            for (int j = 0; j < WIN_SIZE; j++)
                tmp[i][j] = map[i][j];
        }
        for (int i = 1; i < WIN_SIZE - 1; i++) {
            for (int j = 1; j < WIN_SIZE - 1; j++)
                map[i][j] = (tmp[i-1][j] + tmp[i+1][j] + tmp[i][j-1] + tmp[i][j+1]) * 0.25;
        }

        if (graphic == 1) {
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
}

void run(int nb_iter, int graphic)
{
    std::vector<std::vector<double>> map(WIN_SIZE);
    std::tuple<std::string, int, int64_t> result;
    XlibWrap render;
    Timer t;
    long long end;

    t.start("global");
    t.start("timer");
    for (auto &row : map) row.resize(WIN_SIZE);

    if (graphic == 1)
        render.init();

    init_map(map);
    t.start("timer");
    compute_heat(map, render, nb_iter, graphic);
    end = t.get_elapsed_time("timer", "oui", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("seq", 1, end);

    write_results(result);
    render.destroy();
}

int main(int ac, char **av) {
    if (ac != 3) {
        print_usage();
        return 1;
    }

    int nb_iter = std::atoi(av[1]);
    int graphic = std::atoi(av[2]);

    print_user();
    run(nb_iter, graphic);
}

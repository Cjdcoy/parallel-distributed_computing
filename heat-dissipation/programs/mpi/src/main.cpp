#include <vector>
#include <iostream>
#include <tuple>
#include <map>
#include <iostream>
#include <fstream>

#include "heat_mpi.h"
#include "timer.h"

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
    std::cout << "Assignment 4, heat, mpi implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./heat_mpi NB_ITER GRAPHICAL" << std::endl;
    std::cout << "          NB_ITER: number of iterations" << std::endl;
    std::cout << "          GRAPHICAL: 0 graphics, 1 without graphics" << std::endl;
    std::cout << "EXAMPLE:  ./bin/heat_mpi 1000 12 0\n" << std::endl;
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

void run(int nb_iter, int mpi_id, int graphic)
{
    std::vector<std::vector<double>> map(WIN_SIZE); for (auto &row : map) row.resize(WIN_SIZE);
    std::tuple<std::string, int, int64_t> result;
    XlibWrap render;
    Timer t;
    long long end;
    int mpi_tot_id;

    init_map(map);

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    if (mpi_id == 0) {
        if (graphic == 0)
            render.init();
        t.start("timer");
        mpi_master(render, map, nb_iter, graphic);
        MPI_Finalize();
    }
    else {
        mpi_slave(map, mpi_id, nb_iter);
        MPI_Finalize();
        exit(0);
    }
    end = t.get_elapsed_time("timer", "oui", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("mpi", mpi_tot_id, end);
    write_results(result);
    if (graphic == 0)
        render.destroy();
}

int main(int ac, char **av) {
    int mpi_id;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
    if (ac != 3) {
        if (mpi_id == 0)
            print_usage();
        return 1;
    }

    int nb_iter = std::atoi(av[1]);
    int graphic = std::atoi(av[2]);

    if (mpi_id == 0)
        print_user();
    run(nb_iter, mpi_id, graphic);
    return 0;
}

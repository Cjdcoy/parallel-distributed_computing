#include <vector>
#include <iostream>
#include <tuple>
#include <map>
#include <iostream>
#include <fstream>

#include "nbody_mpi.h"
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
    std::cout << "Assignment 3, nbody, mpi implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./nbody_mpi NB_ITER GRAPHICAL" << std::endl;
    std::cout << "          NB_ITER: number of iterations" << std::endl;
    std::cout << "          GRAPHICAL: 0 for grapihics, 1 without graphics" << std::endl;
    std::cout << "EXAMPLE: mpirun -n 6 ./bin/nbody_mpi 1000 0\n" << std::endl;
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
    myfile.open ("nbody.out", std::ios_base::app);
    myfile << output;
    myfile.close();
}

/**
 *
 * @param iteration
 * @param mpi_id
 */
static void run_graphic(int iteration, int mpi_id)
{
    int mpi_tot_id;
    std::tuple<std::string, int, int64_t> result;
    XlibWrap render;
    Timer t;
    long long end;

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    if (mpi_id == 0) {
        render.init();
        t.start("test");
        mpi_master(render, iteration);
    }
    else {
        mpi_slave(mpi_id, iteration);
        MPI_Finalize();
        exit(0);
    }
    MPI_Finalize();
    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("mpi", mpi_tot_id, end);
    write_results(result);
    render.destroy();
}

/**
 *
 * @param iteration
 * @param mpi_id
 */
static void run(int iteration, int mpi_id)
{
    int mpi_tot_id;
    std::tuple<std::string, int, int64_t> result;
    Timer t;
    long long end;

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    if (mpi_id == 0) {
        t.start("test");
        mpi_master(iteration);
    }
    else {
        mpi_slave(mpi_id, iteration);
        MPI_Finalize();
        exit(0);
    }
    MPI_Finalize();
    end = t.get_elapsed_time("test", "end", true);
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    result = make_tuple("mpi", mpi_tot_id, end);
    write_results(result);
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
    if (graphic == 0)
        run_graphic(nb_iter, mpi_id);
    else
        run(nb_iter, mpi_id);
}

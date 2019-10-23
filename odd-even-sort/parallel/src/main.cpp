//
// Created by Fabien Labarbe-Desvignes on 03/09/19.
//

#include <iostream>
#include <mpi.h>

#include "main.h"
#include "runner.h"
#include "mpi_odd_even_sort.h"

/**
 * prints the program usage
 */
static void print_usage()
{
    std::cout << "USAGE: ./even-odd-sort VECTOR_SIZE SEED MOD [NB_THREAD]" << std::endl;
    std::cout << "          VECTOR_SIZE: size of the vector containing the random numbers that will be generated" << std::endl;
    std::cout << "          SEED: seed on which the random numbers will be generated" << std::endl;
    std::cout << "          MOD: \"mpi\" or \"pthread\"" << std::endl;
    std::cout << "          NB_THREAD: unecessary for mpi, required for pthread (number of threads that will be used\n" << std::endl;
    std::cout << "MPI EXAMPLE:      mpirun -n 6 ./bin/odd-even-sort 10000 42 mpi" << std::endl;
    std::cout << "PTHEAD EXAMPLE:   ./bin/odd-even-sort 10000 42 pthread 6\n" << std::endl;
}

static void print_user()
{
    std::cout << "Name: Fabien Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 1, Parallel Odd-Even Transposition Sort" << std::endl;
}

/**
 *
 * @param ac number of arguments
 * @param av list of arguments
 *
 * verifies that the arguments are valid
 */
void error_handler(int ac, char **av)
{
    if (ac < 4) {
        print_usage();
        exit(1);
    }
    if (std::atoi(av[1]) <= 0)
        exit(1);
}

/**
 *
 * @param list_size size of the vector of random numbers
 * @param mpi_tot_id total number of nodes
 *
 * if there are too many nodes sets compared to the vector size, total number of nodes is set to vector size / 2 - 1
 */
static void mpi_error_handler(int list_size, int *mpi_tot_id)
{
    if (*mpi_tot_id >= (list_size / 2))
        *mpi_tot_id = list_size / 2 - 1;
}

/**
 *
 * @param list_size vector size
 * @param seed this is the seed which whom the random vector will be generated
 *
 * initialize and launch the mpi programs
 */
static void main_mpi(int list_size, int seed)
{
    int mpi_id;
    int mpi_tot_id;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    MPI_Barrier(MPI_COMM_WORLD);

    mpi_error_handler(list_size, &mpi_tot_id);
    if (mpi_id == 0) {
        Runner bench(list_size, seed, mpi_tot_id, "mpi");
        bench.run();
    }
    else if (mpi_id < list_size / 2)
        mpi_slave(list_size, mpi_tot_id);
    MPI_Finalize();
    if (mpi_id != 0)
        exit(0);
}

/**
 *
 * @param list_size vector size
 * @param seed this is the seed which whom the random vector will be generated
 * @param nb_thread number of threads that will be launched
 *
 * initialize and launch the pthread programs
 */
void main_pthread(int list_size, int seed, int nb_thread)
{
    Runner bench(list_size, seed, nb_thread, "pthread");
    bench.run();
}

int main(int ac, char **av)
{
    Timer timer;
    long long end;
    int list_size;
    int seed;

    timer.start("global_timer");
    error_handler(ac, av);
    list_size = std::atoi(av[1]);
    seed = std::atoi(av[2]);

    if (std::string("mpi").compare(av[3]) == 0)
        main_mpi(list_size, seed);
    else if (std::string("pthread").compare(av[3]) == 0)
        main_pthread(list_size, seed, std::atoi(av[4]));
    else {
        print_usage();
        exit(1);
    }
    end = timer.get_elapsed_time("global_timer", "end", true);
    print_user();
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    return (0);
}
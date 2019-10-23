
#include <mpi.h>
#include <utility>
#include <cmath>
#include <map>
#include <utility>
#include <fstream>

#include "mpi_mandlebrot.h"
#include "timer.h"
#include "main.h"

int g_max_iter = 2000;

static void print_user()
{
    std::cout << "Name: Fabien Labarbe-Desvignes" << std::endl;
    std::cout << "Sudent ID: 119100002" << std::endl;
    std::cout << "Assignment 2, mandlebrot, MPI implementation." << std::endl;
}

static void print_usage()
{
    std::cout << "USAGE: ./mandlebrot_mpi NB_ITER MODE" << std::endl;
    std::cout << "          NB_ITER: number of iterations for the mandlebrot " << std::endl;
    std::cout << "          MODE: -1: all modes, 0: line partitioning, 1: pixel partitioning, 2: zone partitioning\n" << std::endl;
    std::cout << "EXAMPLE:  mpirun -n 12 ./bin/mandlebrot_mpi 1000 0\n" << std::endl;
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

/**
 *
 * @param shares Containes the lines numbers on which each thread will work
 * @param x_conditions information containing the start of the x value and the value we'll add when iterating on the lines assigned
 * @param process_nb number of the actual thread
 * @param nb_threads total number of thread
 * @return a vector containing the lines each thread will work on
 *
 * refer to the report to understand the per line partitioning
 */
static std::vector<int> partitioning_per_line(std::vector<int>&shares, std::pair<int, int> &x_conditions, int process_nb, int nb_threads)
{
    x_conditions = make_pair(0, 1);
    for (int idx = process_nb ; idx <= WIN_SIZE ; idx+=nb_threads) {
        shares.push_back(idx);
    }
    return shares;
}

/**
 *
 * @param shares Containes the lines numbers on which each thread will work
 * @param x_conditions information containing the start of the x value and the value we'll add when iterating on the lines assigned
 * @param process_nb number of the actual thread
 * @param nb_threads total number of thread
 * @return a vector containing the lines each thread will work on
 *
 * refer to the report to understand the per pixel partitioning
 */
static std::vector<int> partioning_per_pixel(std::vector<int>&shares, std::pair<int, int> &x_conditions, int process_nb, int nb_threads)
{
    x_conditions = make_pair(process_nb, nb_threads);
    for (int idx = 0 ; idx < WIN_SIZE; idx++) {
        for (int thread = process_nb ; thread < nb_threads; thread+=nb_threads)
            shares.push_back(idx);
    }
    return shares;
}


/**
 *
 * @param shares Containes the lines numbers on which each thread will work
 * @param x_conditions information containing the start of the x value and the value we'll add when iterating on the lines assigned
 * @param process_nb number of the actual thread
 * @param nb_threads total number of thread
 * @return a vector containing the lines each thread will work on
 *
 * refer to the report to understand the per zone partitioning
 */
void partioning_per_zone(std::vector<int>&shares, std::pair<int, int> &x_conditions, int process_nb, int nb_threads)
{
    int share = WIN_SIZE / (nb_threads);

    x_conditions = make_pair(0, 1);
    int idx = share * process_nb;
    for (int start = idx ; start <= (idx + share) ; start++) {
        shares.push_back(start);
    }
}

/**
 *
 * @param shares Containes the lines numbers on which each thread will work
 * @param x_conditions information containing the start of the x value and the value we'll add when iterating on the lines assigned
 * @param process_nb number of the actual thread
 * @param nb_threads total number of thread
 * @param mode defines the partitioning method that will be used
 */
void scheduler(std::vector<int>&shares, std::pair<int, int> &x_conditions, int process_nb, int nb_threads, int mode)
{
    if (mode == 0)
        partitioning_per_line(shares, x_conditions,process_nb, nb_threads);
    else if (mode == 1)
        partioning_per_pixel(shares, x_conditions, process_nb, nb_threads);
    else
        partioning_per_zone(shares, x_conditions, process_nb, nb_threads);
}

/**
 *
 * @param xlib XLIB wrapper
 * @param mode partitioning mode
 * @param mpi_id id of the actual process ID
 */
static void run_mpi(XlibWrap &xlib, int mode, int mpi_id)
{
    Timer t;
    std::vector<std::tuple<std::string, int, int64_t>> results;
    char mode_names[3][10] = {"line", "pixel", "zone"};
    int actual_mode = mode == -1 ? mode + 1 : mode;
    long long end;
    int mpi_tot_id;

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);
    if (mpi_id == 0)
        xlib.init(WIN_SIZE);

    for (; actual_mode < 3 ; actual_mode++) {
        t.start("time");
        std::pair<int, int> x_conditions;
        std::vector<int> shares;
        scheduler(shares, x_conditions, mpi_id, mpi_tot_id, actual_mode);

        if (mpi_id == 0) {
            mpi_master(xlib, shares, x_conditions, g_max_iter);
            end = t.get_elapsed_time("time", "end", true);
            std::cout << "runTime is " << end / (float)1000000 << " seconds (" << mode_names[actual_mode] << ")" << std::endl;
            results.emplace_back(make_tuple(mode_names[actual_mode], mpi_tot_id, end));
        }
        else
            mpi_slave(shares, x_conditions, g_max_iter);
        if (mode != -1)
            break;
    }

    MPI_Finalize();
    if (mpi_id != 0)
        exit(0);
    //write_results(results);
}

int main(int ac, char **av) {
    XlibWrap xlib;
    int mpi_id;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);

    if (ac < 3) {
        if (mpi_id == 0)
            print_usage();
        return 1;
    }
    g_max_iter = atoi(av[1]);
    int mode = atoi(av[2]);

    if (g_max_iter < 0 || (mode < -1 || mode > 2)) {
        if (mpi_id == 0) {
        std::cout << "ARGUMENT ERROR, nb_iter or mode invalid." << std::endl;
        print_usage(); } return 1;
    }
    else if (mpi_id == 0)
        print_user();

    // Function calling
    run_mpi(xlib, mode, mpi_id);
    //without this sleep the pixels don't have the time to show up if the computation's too fast
    usleep(500000);
    xlib.put_image(0, 0, 0, 0, WIN_SIZE, WIN_SIZE);
    xlib.flush();
    sleep(10);
    xlib.destroy();
    return 0;
}
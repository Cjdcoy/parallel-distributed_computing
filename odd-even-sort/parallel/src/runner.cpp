//
// Created by cjdcoy on 04/09/19.
//

#include <unistd.h>

#include <tuple>
#include <fstream>
#include "timer.h"
#include "runner.h"
#include "mpi_odd_even_sort.h"
#include "pthread_odd_even_sort.h"


/**
 *
 * @param content content to display
 * @param size length of the array
 * @param n display u
 */
static void print_data(int *content, int size, int n)
{
    for (int i = 0 ; i < size && i < n ; i++)
        std::cout << content[i] << " ";
    std::cout<<std::endl;
}

/**
 *
 * @param size of the vector that will be generated
 * @param seed with whom the vector will be gerated
 * @return the vector filled with random numbers
 */
std::vector<int> generate_random_vector(size_t size, int seed)
{
    std::vector<int> list(size);

    std::random_device random_device;
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0, UINT_MAX);
    for (auto & number : list)
        number = distribution(generator);
    std::cout << "finished generating a random vector of " << size << " elements" << std::endl;
    return list;
}

/**
 *
 * @param results contains numbers to write
 *
 * writes the performances into a file for a python graph interpretation
 */
void Runner::write_results(std::vector<std::tuple<std::string, int, int64_t>> &results)
{
    std::string output;
    std::ofstream myfile;

    for (auto &result : results) {
        output += std::get<0>(result) + " " + to_string(std::get<1>(result)) + " " + to_string(std::get<2>(result)) + "\n";
    }
    myfile.open ("odd_even_sort.out", std::ios_base::app);
    myfile << output;
    myfile.close();
}

/**
 * runs the mpi master or the threads dependeing of the choosen method
 */
void Runner::run()
{
    Timer local_timer;
    std::vector<std::tuple<std::string, int, int64_t>> results;

    local_timer.start("chrono");

    if (_method.compare("pthread") == 0)
        my_pthread_launcher(_rvect, _nb_node);
    else
        mpi_master(&_rvect[0], _rvect.size(), _nb_node);
    results.emplace_back(make_tuple(_method, _nb_node, local_timer.get_elapsed_time("chrono", "finish", true)));
    Logs::even_odd_sort("writing results...\n", 2);
    write_results(results);
}

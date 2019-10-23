//
// Created by cjdcoy on 04/09/19.
//

#ifndef EVEN_ODD_SORT_BENCHMARK_H
#define EVEN_ODD_SORT_BENCHMARK_H

#include <iostream>
#include <vector>
#include <random>
#include <unistd.h>
#include <climits>
#include "logs.h"

/**
 * the role of this class is to generate the random vector and launch the mpi / pthread processes
 */
class Runner {
public:
    Runner(size_t size, int seed, int nb_node, std::string method) : _rvect(size) {
        _seed = seed;
        _nb_node = nb_node;
        _method = method;
        std::random_device random_device;
        std::mt19937 generator(_seed);
        std::uniform_int_distribution<int> distribution(0, INT_MAX);
        for (auto & number : _rvect)
            number = distribution(generator);

        Logs::even_odd_sort(std::string("finished generating a random vector of " + std::to_string(size) + " elements\n").c_str(), 2);
    };
    void write_results(std::vector<std::tuple<std::string, int, int64_t>> &results);

    const std::vector<int> &get_vector() const { return _rvect; }
    const std::vector<int> get_vector_copy() const { return _rvect; }
    void run();

private:
    unsigned int _seed;
    unsigned int _nb_node;
    std::vector <int> _rvect;
    std::string _method;
};


#endif //EVEN_ODD_SORT_BENCHMARK_H

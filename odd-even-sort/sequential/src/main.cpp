//
// Created by Fabien Labarbe-Desvignes on 03/09/19.
//

#include <iostream>
#include "timer.h"
#include "main.h"
#include "runner.h"
#include "odd_even_sort.h"

/**
 * prints the program usage
 */
static void print_usage()
{
    std::cout << "USAGE: ./even-odd-sort VECTOR_SIZE SEED" << std::endl;
    std::cout << "          VECTOR_SIZE: size of the vector containing the random numbers that will be generated" << std::endl;
    std::cout << "          SEED: seed on which the random numbers will be generated\n" << std::endl;
    std::cout << "EXAMPLE:      ./bin/odd-even-sort 10000 42" << std::endl;
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
    if (ac != 3) {
        print_usage();
        exit(1);
    }
    if (std::atoi(av[1]) <= 0)
        exit(1);
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

    Runner bench(list_size, seed, 0, "sequential");
    bench.run();

    end = timer.get_elapsed_time("global_timer", "end", true);
    print_user();
    std::cout << "runTime is " << end / (float)1000000 << " seconds" << std::endl;
    return (0);
}
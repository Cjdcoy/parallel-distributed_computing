//
// Created by cjdcoy on 06/10/19.
//

#include "odd_even_sort.h"
#include <iostream>

/**
 *
 * @param content content to display
 * @param size length of the array
 * @param n display until n
 */
static void print_data(int *content, int size, int n)
{
    for (int i = 0 ; i < size && i < n ; i++)
        std::cout << content[i] << " ";
    std::cout<<std::endl;
}

/**
 *
 * @param array
 * @param array_size
 */
void odd_even_sort(int *array, int array_size)
{
    bool sorted = false;

    std::cout << "random array: ";
    print_data(array, array_size, 20);
    while (1) {
        sorted = true;

        for (int i = 1; i <= array_size - 2; i += 2) {
            if (array[i] > array[i + 1]) {
                std::swap(array[i], array[i + 1]);
                sorted = false;
            }
        }
        for (int i = 0; i <= array_size - 2; i += 2) {
            if (array[i] > array[i + 1]) {
                std::swap(array[i], array[i + 1]);
                sorted = false;
            }
        }
        if (sorted == true)
            break;
    }
    std::cout << "sorted array: ";
    print_data(array, array_size, 20);
}
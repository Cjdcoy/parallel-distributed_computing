//
// Created by cjdcoy on 17/10/19.
//

#include "mpi_mandlebrot.h"
#include <cmath>
#include <map>


/**
 *
 * @param content content to print
 * @param size size of the array
 * @param n print until n
 *
 *
 */
static void print_data(int *content, int size, int n)
{
    for (int i = 0 ; i < size && i < n ; i++)
        std::cout << content[i] << " ";
    std::cout<<std::endl;
}

typedef struct complextype
{
    float real, imag;
} Compl;

unsigned int return_pixel(int r, int g, int b) {
    return r * 65536 + g * 256 + b;
}

/**
 *
 * @param lines lines on which the worker will work
 * @param x_conditions information containing the start of the x value and the value we'll add when iterating on the lines assigned
 * @param map vector containing the x, y position of each computed pixel and its value
 * @param max_iter maximum number of iteration to perform while computing the mandlebrot
 *
 * here the value of the mandlebrot set will be computed for each worker and put insde a vector.
 */
void parallel_mandlebrot(std::vector<int> &lines, std::pair<int, int> &x_conditions, std::vector<int> &map, int max_iter)
{
    Compl   z, c;
    int k = 0;
    float ratio1 = WIN_SIZE / (float)2;
    float ratio2 = WIN_SIZE / (float)4;

    for (auto y : lines) {
        for (int x = x_conditions.first ; x < WIN_SIZE ; x+= x_conditions.second) {
            z.real = z.imag = 0.0;
            c.real = ((float) x - ratio1)/(ratio2);
            c.imag = ((float) y - ratio1)/(ratio2);
            k = 0;
            double  lengthsq = 0, temp = 0;
            while (lengthsq < 20 && k < max_iter)    {

                temp = z.real*z.real - z.imag*z.imag + c.real;
                z.imag = 2.0*z.real*z.imag + c.imag;
                z.real = temp;
                lengthsq = z.real*z.real+z.imag*z.imag;
                // this condition is for optimization purpose, only the last computed pixel will be added to our vector sor we donc ompute needless values.
                if (k == max_iter && k > 0) {
                    map.emplace_back(x);
                    map.emplace_back(y);
                    map.emplace_back(return_pixel(-(std::sqrt(z.real / (float) max_iter) * 155), 0 , 120));
                }
                k++;
            }
            if (k != max_iter && k > 0) {
                map.emplace_back(x);
                map.emplace_back(y);
                map.emplace_back(return_pixel(-(std::sqrt(z.real / (float) max_iter) * 155), 0 , 120));
            }
        }
    }
}

/**
 *
 * @param xlib Xlib wrapper
 * @param shares contains the lines this processus will work on
 * @param x_conditions information containing the start of the x value and the value we'll add when iterating on the lines assigned
 * @param max_iter maximum number of iteration to perform while computing the mandlebrot
 *
 * Once its job is finished, the master wait to receive the other parts of the mandlebrot set that have been calculated.
 */
void mpi_master(XlibWrap &xlib, std::vector<int>&shares, std::pair<int, int> &x_conditions, int max_iter)
{
    int mpi_id; MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
    int mpi_tot_id;
    int idx= 0;
    int r_size = 0;
    MPI_Status status;
    std::vector<int> map;
    std::vector<int> r_buf(WIN_SIZE * WIN_SIZE * 3);

    //gather_data(&container);
    parallel_mandlebrot(shares, x_conditions, map, max_iter);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_tot_id);

    for (int i = 1; i < mpi_tot_id ; i++) {
        MPI_Status status;
        // Probe for an incoming message from process zero
        MPI_Probe(i, 0, MPI_COMM_WORLD, &status);

        // When probe returns, the status object has the size and other
        // attributes of the incoming message. Get the message size
        MPI_Get_count(&status, MPI_INT, &r_size);

        // Now receive the message with the allocated buffer
        // Allocate a buffer to hold the incoming numbers
        MPI_Recv(&r_buf[0], WIN_SIZE * WIN_SIZE * 3, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0 ; i + 3 <= r_size ; i += 3) {
            xlib.put_pixel(r_buf[i], r_buf[i + 1], r_buf[i + 2]);
        }
    }

    for (int i = 0 ; i + 3 <= map.size() ; i+=3) {
        xlib.put_pixel(map[i], map[i + 1], map[i + 2]);
    }
}

/**
 *
 * @param shares contains the lines this processus will work on
 * @param x_conditions information containing the start of the x value and the value we'll add when iterating on the lines assigned
 * @param max_iter maximum number of iteration to perform while computing the mandlebrot
 *
 * Once the slave finished computing the mandlebrot set, it sends back the result to the master
 */
void mpi_slave(std::vector<int>&shares, std::pair<int, int> &x_conditions, int max_iter)
{
    int mpi_id; MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
    std::vector<int> map;
    Timer t;

    parallel_mandlebrot(shares, x_conditions, map, max_iter);
    MPI_Ssend(&map[0], map.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
}
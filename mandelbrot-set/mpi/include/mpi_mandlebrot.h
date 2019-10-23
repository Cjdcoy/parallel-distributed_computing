//
// Created by cjdcoy on 17/10/19.
//

#ifndef MANDLEBROT_MPI_MANDLEBROT_H
#define MANDLEBROT_MPI_MANDLEBROT_H

#include <condition_variable>
#include <atomic>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <mpi.h>
#include <timer.h>
#include "x11-helpers.h"


#define WIN_SIZE 200

typedef struct data_s {
    //root array and information
    int *array = NULL;
    int size_array;
    int mpi_id;

    //global information about the array distribution
    int *elem_per_proc = NULL;
    int *offset = NULL;

    //global informations
    int *global_array = NULL;
    int global_array_size;
    int mpi_tot_id;
} data_t;

void mpi_master();
void mpi_master(XlibWrap &xlib, std::vector<int>&shares, std::pair<int, int> &x_conditions, int);
void mpi_slave(std::vector<int>&shares, std::pair<int, int> &x_conditions, int max_iter);

#endif //MANDLEBROT_MPI_MANDLEBROT_H

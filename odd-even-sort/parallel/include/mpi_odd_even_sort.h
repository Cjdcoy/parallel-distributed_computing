//
// Created by cjdcoy on 19/09/19.
//

#ifndef EVEN_ODD_SORT_MPI_ODD_EVEN_SORT_H
#define EVEN_ODD_SORT_MPI_ODD_EVEN_SORT_H

#include <condition_variable>
#include <atomic>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <mpi.h>
#include <timer.h>

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

void mpi_master(int *rvect, int, int);
void mpi_slave(int, int);

#endif //EVEN_ODD_SORT_MPI_ODD_EVEN_SORT_H

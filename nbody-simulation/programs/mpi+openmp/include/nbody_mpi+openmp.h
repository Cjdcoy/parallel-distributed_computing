//
// Created by cjdcoy on 08/11/19.
//

#ifndef NBODY_MPI_NBODY_MPI_H
#define NBODY_MPI_NBODY_MPI_H

#include <mpi.h>
#include "main.h"
#include "body.h"

void mpi_master(XlibWrap &xlib, int, int );
void mpi_master(int, int);
void mpi_slave(int mpi_id, int, int);

#endif //NBODY_MPI_NBODY_MPI_H

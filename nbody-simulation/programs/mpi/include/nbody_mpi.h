//
// Created by cjdcoy on 08/11/19.
//

#ifndef NBODY_MPI_NBODY_MPI_H
#define NBODY_MPI_NBODY_MPI_H

#include <mpi.h>
#include "body.h"

void mpi_master(XlibWrap &xlib, int);
void mpi_master(int);
void mpi_slave(int mpi_id, int);

#endif //NBODY_MPI_NBODY_MPI_H

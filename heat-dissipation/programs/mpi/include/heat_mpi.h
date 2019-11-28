//
// Created by cjdcoy on 08/11/19.
//

#ifndef NBODY_MPI_NBODY_MPI_H
#define NBODY_MPI_NBODY_MPI_H

#include <mpi.h>
#include "x11-helpers.h"

void mpi_master(XlibWrap &xlib, std::vector<std::vector<double>>  &map, int nb_iteration, int graphic);
void mpi_slave(std::vector<std::vector<double>>  &map, int mpi_id, int nb_iteration);
#endif //NBODY_MPI_NBODY_MPI_H

# HPC simulation

Parallelization of a simulation of temperature variation on a metal plate, developed entirely in C.

The developement includes the necessary functionality to export the results (in terms of processing stats and time spent) to CSV in order to perform a posterior study of the efficiency of the libraries used.

The following have been used for this purpose
- phtreads
- MPI
- OpenMP
- Hybrid between MPI and OpenMP

**NOTE**: If it is necessary to see the results between iterations, change in `utils.h` the value of the constant `LOG_LEVEL` to `DEBUG`.

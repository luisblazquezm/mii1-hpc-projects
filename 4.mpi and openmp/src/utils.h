#include "logging.h"

#ifndef __UTILS_H
#define __UTILS_H

#define NUM_PROC 1
#define NUM_ROWS 10
#define NUM_COLS 10
#define NUM_THREADS 10
#define LOG_LEVEL ERROR
#define NUM_ITERATIONS 1000

#define IO_ERROR_CODE 1

#define CSV_FILE "../results.csv"
#define ENTRY_TYPE "mpi_openmp"

#define MASTERID (0)
#define ISMASTER(id) ( (id) == MASTERID )

#endif
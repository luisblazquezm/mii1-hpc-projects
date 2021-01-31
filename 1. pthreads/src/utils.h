#include "logging.h"

#ifndef __UTILS_H
#define __UTILS_H

#define SIMULATION_SERIAL 0
#define SIMULATION_PARALELL 1
#define SELECTED_SIMULATION SIMULATION_PARALELL

#define NUM_ROWS 10
#define NUM_COLS 10
#define NUM_THREADS 2
#define LOG_LEVEL ERROR
#define NUM_ITERATIONS 100

#define IO_ERROR_CODE 1

#define CSV_FILE "../results.csv"
#define ENTRY_TYPE "pthreads"

#endif
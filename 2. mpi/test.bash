#!/bin/bash

MIN_NUM_ROWS=100
MAX_NUM_ROWS=1000

MIN_NUM_COLS=100
MAX_NUM_COLS=1000

MIN_NUM_ITERATIONS=1000
MAX_NUM_ITERATIONS=1000

MIN_PROCS=1
MAX_PROCS=8

# allow mpi permissions
export OMPI_MCA_btl_vader_single_copy_mechanism=none

# move to sources and compile
cd src
make clean
make

# paralel simulation
for ((numIterations = $MIN_NUM_ITERATIONS; numIterations <= $MAX_NUM_ITERATIONS; numIterations++)); do
	for ((numRows = $MIN_NUM_ROWS; numRows <= $MAX_NUM_ROWS; numRows+=100)); do
		for ((numCols = $MIN_NUM_COLS; numCols <= $MAX_NUM_COLS; numCols+=100)); do
			for ((numProcs = $MIN_PROCS; numProcs <= $MAX_PROCS; numProcs++)); do
				echo "paralell ${numIterations} iterations simulation with ${numProcs} procs and ${numRows}x${numCols} matrix"
				echo "args: ($numProcs) $numRows $numCols $numIterations"
				mpirun -np "$numProcs" ./main "$numRows" "$numCols" "$numIterations"
			done
		done
	done
done

make clean
cd ..
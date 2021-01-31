#!/bin/bash

MIN_NUM_ROWS=100
MAX_NUM_ROWS=1000

MIN_NUM_COLS=100
MAX_NUM_COLS=1000

MIN_NUM_ITERATIONS=1000
MAX_NUM_ITERATIONS=1000

MIN_PROCS=1
MAX_PROCS=6

MIN_THREAD=1
MAX_THREAD=20

NUM_DIVISIONS=1
DIVISIONS=(2 3)
DIVISIONS_STR=("column" "row")

# allow mpi permissions
export OMPI_MCA_btl_vader_single_copy_mechanism=none

# move to sources and compile
cd src
make clean
make

for ((numIterations = $MIN_NUM_ITERATIONS; numIterations <= $MAX_NUM_ITERATIONS; numIterations++)); do
	for ((numRows = $MIN_NUM_ROWS; numRows <= $MAX_NUM_ROWS; numRows+=100)); do
		for ((numCols = $MIN_NUM_COLS; numCols <= $MAX_NUM_COLS; numCols+=100)); do
			for ((numProcs = $MIN_PROCS; numProcs <= $MAX_PROCS; numProcs++)); do
				for ((numThreads = $MIN_THREAD; numThreads <= $MAX_THREAD; numThreads++)); do
					for ((div = 0; div <= $NUM_DIVISIONS; div++)); do
						echo "${numIterations} iterations simulation with ${numProcs} P x ${numThreads} Th, ${DIVISIONS_STR[div]} divisions and ${numRows}x${numCols} matrix"
						echo "args: (-np $numProcs) $numRows $numCols $numIterations $numThreads ${DIVISIONS[div]}"
						mpirun -np "$numProcs" ./main "$numRows" "$numCols" "$numIterations" "$numThreads" "${DIVISIONS[div]}"
					done
				done
			done
		done
	done
done


make clean
cd ..

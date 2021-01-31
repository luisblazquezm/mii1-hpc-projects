#!/bin/bash

SERIAL_SIMULATION=0
PARALELL_SIMULATION=1

MIN_NUM_ROWS=100
MAX_NUM_ROWS=1000

MIN_NUM_COLS=100
MAX_NUM_COLS=1000

MIN_NUM_ITERATIONS=1000
MAX_NUM_ITERATIONS=1000

MIN_THREADS=1
MAX_THREADS=20

NUM_DIVISIONS=1
DIVISIONS=(2 3)
DIVISIONS_STR=("column" "row")

# move to sources and compile
cd src
make clean
make

# paralel simulation
for ((numIterations = $MIN_NUM_ITERATIONS; numIterations <= $MAX_NUM_ITERATIONS; numIterations++)); do
	for ((numRows = $MIN_NUM_ROWS; numRows <= $MAX_NUM_ROWS; numRows+=100)); do
		for ((num_th = $MIN_THREADS; num_th <= $MAX_THREADS; num_th++)); do
			for ((div = 0; div <= $NUM_DIVISIONS; div++)); do
				echo "paralell ${numIterations} iterations simulation with ${num_th} threads and ${numRows}x${numRows} matrix with ${DIVISIONS_STR[div]} divisions"
				echo "args: $numRows $numRows $num_th $numIterations $SERIAL_SIMULATION ${DIVISIONS[div]}"
				./main "$numRows" "$numRows" "$num_th" "$numIterations" "$PARALELL_SIMULATION" ${DIVISIONS[div]}
			done
		done
	done
done

make clean
cd ..
CC=gcc
CFLAGS=-fopenmp -g
COFLAGS=-fopenmp -g -c
OBJ_TARGETS= simulation.o plate.o logging.o paralellUtils.o csv.o

main: main.c utils.h csv.h $(OBJ_TARGETS)
	$(CC) $(CFLAGS) main.c $(OBJ_TARGETS) -o main 

simulation.o: simulation.c simulation.h plate.o plate.h paralellUtils.o paralellUtils.h logging.h
	$(CC) $(COFLAGS) simulation.c -fopenmp

paralellUtils.o: paralellUtils.c plate.c plate.h
	$(CC) $(COFLAGS) paralellUtils.c

plate.o: plate.c utils.h logging.h
	$(CC) $(COFLAGS) plate.c

logging.o: logging.c logging.h utils.h
	$(CC) $(COFLAGS) logging.c

csv.o: csv.h csv.c utils.h paralellUtils.h
	$(CC) $(COFLAGS) csv.c

clean:
	rm *.o
	rm main
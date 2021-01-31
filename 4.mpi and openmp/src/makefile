MPICC=mpicc
CMPILAGS=-g -c -fopenmp
OBJS=simulation.o plate.o logging.o paralellUtils.o csv.o paralellThreadUtils.o

main: main.c utils.h $(OBJS)
	$(MPICC) main.c $(OBJS) -o main -fopenmp

simulation.o: simulation.c simulation.h
	$(MPICC) $(CMPILAGS) simulation.c

paralellUtils.o: paralellUtils.c paralellUtils.h
	$(MPICC) $(CMPILAGS) paralellUtils.c

paralellThreadUtils.o: paralellThreadUtils.c paralellThreadUtils.h
	$(MPICC) $(CMPILAGS) paralellThreadUtils.c

plate.o: plate.c plate.h
	$(MPICC) $(CMPILAGS) plate.c

logging.o: logging.c logging.h
	$(MPICC) $(CMPILAGS) logging.c

csv.o: csv.c csv.h
	$(MPICC) $(CMPILAGS) csv.c

clean:
	rm -f *.o
	rm -f main

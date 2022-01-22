TARGET = distance_cell.c
#TARGET = distance_cell_timing.c
CC = gcc
LIBS = /home/hpcuser041/lib
INCLUDE = /home/hpcuser041/include
FLAGS = -fopenmp -march=native -std=c11 -Wall -Ofast -lm

cell_distance : $(TARGET)
	$(CC) $(TARGET) -I$(INCLUDE) -L$(LIBS) -o $@ $(FLAGS)
clean:
	rm -f cell_distance

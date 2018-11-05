TARGET = newton.c
CC = gcc
LIBS = /home/hpcuser041/lib
INCLUDE = /home/hpcuser041/include
Flags = -larb -lflint -march=native -std=c11 -Wall -O2

newton : $(Target)
	$(CC) $(TARGET) -I$(INCLUDE) -L$(LIBS) -o $@ $(FLAGS)

CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -g

proj1: proj1.o
	$(CC) $(CFLAGS) -o $@ $^

proj1.o: proj1.c proj1.h
	$(CC) $(CFLAGS) -c proj1.c

clean :
	$(RM) proj1
	$(RM) *.o
CC = gcc
CFLAGS = -Wall -pedantic -g
MAIN = sfind
OBJS = sfind.o
all : $(MAIN)

$(MAIN) : $(OBJS) sfind.h
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

sfind.o : sfind.c sfind.h
	$(CC) $(CFLAGS) -c sfind.c

clean :
	rm *.o $(MAIN) core

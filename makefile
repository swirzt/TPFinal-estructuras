CC=gcc
FLAGS=-std=c99 -pedantic -Wall

all: main check

colasenlazadas.o: colasenlazadas.c colasenlazadas.h
	$(CC) -c colasenlazadas.c $(FLAGS)

main: colasenlazadas.o main.c
	$(CC) -o main main.c colasenlazadas.o $(FLAGS)

slist.o: slist.c slist.h
	$(CC) -c slist.c $(FLAGS)

check: slist.o revisaentrada.c
	$(CC) -o check revisaentrada.c slist.o $(FLAGS)

clean:
	rm -rf colasenlazadas.o
	rm -rf main
	wm -rf slist.o
	rm -rf check
CC=gcc
FLAGS= -std=c99 -pedantic -Wall

all: main

colasenlazadas.o: colasenlazadas.c colasenlazadas.h
	$(CC) -c colasenlazadas.c $(FLAGS)

main: colasenlazadas.o main.c
	$(CC) -o main main.c colasenlazadas.o $(FLAGS)

clean:
	rm -rf colasenlazadas.o
	rm -rf main
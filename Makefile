all: client

client: main.o fonctions.o
	gcc main.o fonctions.o -o client -w

main.o: main.c fonctions.h
	gcc -c -Wall main.c

fonctions.o: fonctions.c definitions.h fonctions.h
	gcc -c -Wall fonctions.c

run: 
	./client localhost 20000



all: ./bin/client

./bin/client: ./bin/main.o ./bin/fonctions.o
	gcc ./bin/main.o ./bin/fonctions.o -o ./bin/client -w

./bin/main.o: ./sources/main.c ./headers/fonctions.h
	gcc -c -Wall ./sources/main.c -o ./bin/main.o

./bin/fonctions.o: ./sources/fonctions.c ./headers/definitions.h ./headers/fonctions.h
	gcc -c -Wall ./sources/fonctions.c -w -o ./bin/fonctions.o

run: 
	./bin/client localhost 2000

clean:
	rm -f ./bin/main.o ./bin/fonctions.o ./bin/client

CC=gcc
HDIR=-I'./header/'

RHJ:	./src/testing.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c ./src/parser.c
	$(CC) -o $@.out ./src/testing.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c ./src/parser.c $(HDIR)

clean:
	$(RM) *.out
	$(RM) ./header/*.h.gch

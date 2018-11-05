CC=gcc
HDIR=-I'./header/'

RHJ:	./src/RHJ.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c
	$(CC) -o $@.out ./src/RHJ.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c $(HDIR)

clean:
	$(RM) *.out
	$(RM) ./header/*.h.gch

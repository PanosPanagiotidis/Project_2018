CC=gcc

prjct1:	eg.c helper_functions.c
	$(CC) -o $@.out eg.c helper_functions.c

bench: benchmark.c randarr.c
	$(CC) -o $@.out benchmark.c randarr.c

daindex: daindex.c
	$(CC) -o $@.out daindex.c

clean:
	$(RM) *.out
	$(RM) *.h.gch

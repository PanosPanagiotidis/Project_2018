CC=gcc

prjct1:	eg.c helper_functions.c
	$(CC) -o $@.out eg.c helper_functions.c

bench: benchmark.c randarr.c
	$(CC) -o $@.out benchmark.c randarr.c

helper:

clean:
	$(RM) prjct.out bench.out
	$(RM) *.h.gch

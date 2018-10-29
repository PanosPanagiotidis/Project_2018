CC=gcc

prjct1:	eg.c
	$(CC) -o $@.out eg.c

bench: benchmark.c randarr.c
	$(CC) -o $@.out benchmark.c randarr.c

clean:
	$(RM) prjct.out bench.out
	$(RM) *.h.gch

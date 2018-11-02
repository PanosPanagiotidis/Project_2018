CC=gcc

RHJ:	bucketing.c helper_functions.c	randarr.c
	$(CC) -o $@.out bucketing.c helper_functions.c	randarr.c -ggdb3

bench: benchmark.c randarr.c
	$(CC) -o $@.out benchmark.c randarr.c -ggdb3

daindex: daindex.c
	$(CC) -o $@.out daindex.c -ggdb3

clean:
	$(RM) *.out
	$(RM) *.h.gch

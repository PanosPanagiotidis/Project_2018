CC=g++
HDIR=-I'./header/'

RHJ:	./src/testing.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c ./src/parsing.cpp ./src/relation_l.cpp
	$(CC) -o $@.out ./src/testing.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c ./src/parsing.cpp	./src/relation_l.cpp  -g3 $(HDIR)

NICK:	./src/testing.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c ./src/parsing.cpp ./src/relation_l.cpp
	$(CC) -o $@.out ./src/testing.c ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c ./src/parsing.cpp	./src/relation_l.cpp ./src/relationops.cpp  -g3 $(HDIR)


clean:
	$(RM) *.out
	$(RM) ./header/*.h.gch

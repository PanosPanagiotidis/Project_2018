CC=g++
HDIR=-I'./header/'

NICK:	./src/testmain.cpp ./src/helper_functions.c ./src/randarr.c ./src/results.c ./src/daindex.c ./src/parsing.cpp ./src/relation_l.cpp
	$(CC) -o $@.out ./src/testmain.cpp ./src/helper_functions.c ./src/randarr.cpp ./src/results.c ./src/daindex.c ./src/parsing.cpp	./src/relation_l.cpp ./src/relationops.cpp  -g $(HDIR)

RHJ:	./src/testing.c ./src/helper_functions.c ./src/randarr.cpp ./src/results.c ./src/daindex.c ./src/parsing.cpp ./src/relation_l.cpp
	$(CC) -o $@.out ./src/testing.c ./src/helper_functions.c ./src/randarr.cpp ./src/results.c ./src/daindex.c ./src/parsing.cpp	./src/relation_l.cpp  -g3 $(HDIR)



clean:
	$(RM) *.out
	$(RM) ./header/*.h.gch

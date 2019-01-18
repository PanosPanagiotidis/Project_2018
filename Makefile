CC=g++
HDIR=-I'./header/'
CFLAGS = -g -o3
CPPFLAGS = -g -o3

all: ./src/testmain.o ./src/helper_functions.o ./src/results.o ./src/daindex.o ./src/parsing.o ./src/relationops.o ./src/relation_l.o ./src/thread_scheduler.o ./src/joinenum.o
	$(CC) $(CFLAGS)	-o JOIN.out ./src/testmain.o ./src/helper_functions.o ./src/results.o ./src/daindex.o ./src/parsing.o ./src/relationops.o ./src/relation_l.o ./src/thread_scheduler.o ./header/includes.h ./src/joinenum.o -lpthread

testmain.o:	./src/testmain.cpp
	$(CC) $(CPPFLAGS) -c ./src/testmain.cpp

helper_functions.o:	./src/helper_functions.cpp
	$(CC) $(CPPFLAGS) -c ./src/helper_functions.cpp

thread_scheduler.o: ./src/thread_scheduler.cpp ./header/includes.h
	$(CC) $(CPPFLAGS) -c ./src/thread_scheduler.cpp ./header/includes.h -lpthread

results.o:	./src/results.cpp ./header/daindex.h
	$(CC) $(CPPFLAGS) -c ./src/results.cpp ./header/includes.h

daindex.o: 	./src/daindex.c
	$(CC) $(CFLAGS) -c ./src/daindex.c

parsing.o:	./src/parsing.cpp
	$(CC) $(CPPFLAGS) -c ./src/parsing.cpp

relation_l.o:	./src/relation_l.cpp
	$(CC) $(CPPFLAGS) -c ./src/relation_l.cpp

relationops.o: ./src/relationops.cpp
	$(CC) $(CPPFLAGS) -c ./src/relationops.cpp

joinenum.o: 	./src/joinenum.cpp
	$(CC) $(CPPFLAGS) -c ./src/joinenum.cpp


clean:
	$(RM) *.out
	$(RM) ./src/*.o
	$(RM) ./header/*.h.gch

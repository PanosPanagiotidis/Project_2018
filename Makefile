CC=g++
HDIR=-I'./header/'
OBJDIR='./obj/'
SRCDIR='./src/'
CPPFLAGS = -o3 $(HDIR)

all: HARNESS JOIN

HARNESS: ./Harness/harness.o
	$(CC) $(CPPFLAGS) -o ./Harness/harness ./Harness/harness.o

harness.o:	./Harness/harness.cpp
	$(CC) $(CPPFLAGS) -c ./Harness/harness.cpp

JOIN: ./src/join.o ./src/helper_functions.o ./src/results.o ./src/daindex.o ./src/parsing.o ./src/relationops.o ./src/relation_l.o ./src/thread_scheduler.o ./src/joinenum.o
	$(CC) $(CPPFLAGS) -o JOIN.out ./src/join.o ./src/helper_functions.o ./src/results.o ./src/daindex.o ./src/parsing.o ./src/relationops.o ./src/relation_l.o ./src/thread_scheduler.o ./header/includes.h ./src/joinenum.o -lpthread

thread_scheduler.o: ./src/thread_scheduler.cpp ./header/includes.h
	$(CC) $(CPPFLAGS) -c ./src/thread_scheduler.cpp ./header/includes.h -lpthread

%.o: $(SRCDIR)/%.cpp
	g++ $(CPPFLAGS) -o $@ -c $<

clean:
	$(RM) *.out
	$(RM) ./src/*.o
	$(RM) ./header/*.h.gch
	$(RM) ./Harness/*.o
	$(RM) ./Harness/harness

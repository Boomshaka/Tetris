CXX = clang++
#CXX = g++

CXXFLAGS = -std=c++11 -Wall -g

BINARIES = tetris

all: ${BINARIES}

clean:
	/bin/rm -f *.o ${BINARIES}
tetris: tetris.o
	${CXX} $^ -o  $@ -lncurses
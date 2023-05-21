.SUFFIXES: .cpp
CXX ?= g++
CXXFLAGS ?= -g -std=c++2b -Wall -pedantic -Wextra # add -O4
CPP = bigint.cpp # ovverride previous definition of ${CPP} so don't replace = with ?=
BIGINT ?= ${CPP} bigint.h
EXEC ?= bigint
OBJ ?= bigint.o test.o

TEST ?= test.cpp
TEST_CXXFLAGS ?= -g -std=c++2b
TEST_EXEC ?= test

bigint: ${EXEC}
	${CXX} ${CXXFLAGS} ${CPP} -o ${EXEC}

test: ${TEST_EXEC}
	${CXX} ${CXXFLAGS} -c ${CPP}
	${CXX} ${TEST_CXXFLAGS} -c ${TEST}
	${CXX} bigint.o test.o -o ${TEST_EXEC}

clean:
	rm -rf ${EXEC} ${TEST_EXEC} ${OBJ}

run-bigint:
	${CXX} ${CXXFLAGS} ${CPP} -o ${EXEC} && ./bigint

run-test:
	${CXX} ${TEST_CXXFLAGS} ${CPP} ${TEST} -o ${TEST_EXEC} && ./test

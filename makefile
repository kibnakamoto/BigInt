.SUFFIXES: .cpp
CXX ?= g++
CXXFLAGS ?= -g -std=c++2b -Wall -pedantic -Wextra # add -O4
CPP = main.cpp # ovverride previous definition of ${CPP} so don't replace = with ?=
BIGINT_CPP ?= bigint.cpp
BIGINT ?= ${BIGINT_CPP} bigint.h ${CPP}
EXEC ?= bigint
BIGINT_OBJ ?= bigint.o
TEST_OBJ ?= test.o
OBJ ?= ${BIGINT_OBJ} ${TEST_OBJ}

TEST ?= test.cpp
TEST_CXXFLAGS ?= -g -std=c++2b
TEST_EXEC ?= test

bigint: ${EXEC}
	${CXX} ${CXXFLAGS} ${CPP} -o ${EXEC}

test: ${TEST_EXEC}
	${CXX} ${CXXFLAGS} -c ${BIGINT_CPP} -o ${BIGINT_OBJ}
	${CXX} ${TEST_CXXFLAGS} -c ${TEST} -o ${TEST_OBJ}
	${CXX} bigint.o test.o -o ${TEST_EXEC}

clean:
	rm -rf ${EXEC} ${TEST_EXEC} ${OBJ}

run-bigint:
	${CXX} ${CXXFLAGS} ${CPP} -o ${EXEC} && ./bigint

run-test:
	${CXX} ${TEST_CXXFLAGS} ${CPP} ${TEST} -o ${TEST_EXEC} && ./test

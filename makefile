.SUFFIXES: .cpp
CXX ?= g++
CXXFLAGS ?= -g -std=c++2b -Wall -pedantic -Wextra # add -O4
CPP = bigint.cpp
BIGINT ?=  ${CPP} bigint.h
EXEC ?= bigint

TEST ?= test.cpp
TEST_CXXFLAGS ?= -g -std=c++2b
TEST_EXEC ?= test

bigint: ${EXEC}
	${CXX} ${CXXFLAGS} ${CPP} -o ${EXEC}

test: ${TEST_EXEC}
	${CXX} ${TEST_CXXFLAGS} ${BIGINT} ${TEST} -o ${TEST_EXEC}

clean:
	rm -rf ${EXEC} ${TEST_EXEC}

run-bigint:
	${CXX} ${CXXFLAGS} ${CPP} -o ${EXEC} && ./bigint

run-test:
	${CXX} ${TEST_CXXFLAGS} ${BIGINT} ${TEST} -o ${TEST_EXEC} && ./test

# Stage3/Makefile
CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -I../Stage1 -I../Stage2

BIN := euler_app
SRCS := ../Stage1/graph.cpp ../Stage2/euler.cpp main.cpp

all: $(BIN)

$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN)

clean:
	rm -f $(BIN)

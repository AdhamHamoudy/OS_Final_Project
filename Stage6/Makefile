CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -I../Stage1 -I../Stage2 -I.
BIN := euler_server
SRCS := ../Stage1/graph.cpp ../Stage2/euler.cpp euler_server.cpp
all: $(BIN)
$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN)
clean:
	rm -f $(BIN)

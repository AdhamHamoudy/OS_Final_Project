# Compiler and flags
CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -g -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnull-dereference -Wformat=2 -Wundef -Wpointer-arith -pthread
LDFLAGS := -pthread
ARGS ?= 5555   # default port

# Include paths to Stage1–3 and Stage6 (for server_protocol.hpp)
INC := -I../Stage1 -I../Stage2 -I../Stage3 -I../Stage6

# Sources
SRC := alg_server.cpp algorithms.cpp ../Stage1/graph.cpp ../Stage2/euler.cpp
TARGET := alg_server

# Tools for coverage/profiling
GCOV_FLAGS := --coverage

.PHONY: all clean run valgrind-memcheck coverage kill-port

# Default target
all: $(TARGET)

# Build server
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INC) $(SRC) -o $@ $(LDFLAGS)

# Kill any process bound to the port
kill-port:
	-fuser -k $(ARGS)/tcp 2>/dev/null || true

# Run server (kills old one first)
run: all kill-port
	./$(TARGET) $(ARGS)

# Clean build artifacts
clean:
	rm -f $(TARGET) *.gcda *.gcno *.info gmon.out callgrind.out.* gprof_report.txt
	rm -rf html coverage

# Valgrind memory check
valgrind-memcheck: all kill-port
	valgrind --leak-check=full --track-origins=yes ./$(TARGET) $(ARGS)

# Code coverage (LCOV)
coverage: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) $(GCOV_FLAGS)" LDFLAGS="$(LDFLAGS) $(GCOV_FLAGS)"
	-./$(TARGET) $(ARGS) & pid=$$!; \
	sleep 2; kill $$pid 2>/dev/null || true
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory html
	@echo "Coverage report available in html/index.html"

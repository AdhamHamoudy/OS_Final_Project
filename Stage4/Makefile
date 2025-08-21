# Compiler and flags
CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -g -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnull-dereference -Wformat=2 -Wundef -Wpointer-arith -pthread
LDFLAGS := -pthread
ARGS ?= -f ../g_ok5.txt

# Source files (relative to Stage4)
SRC := ../Stage1/graph.cpp ../Stage2/euler.cpp ../Stage3/main.cpp
INC := -I../Stage1 -I../Stage2 -I../Stage3
TARGET := euler_app

# Tools for coverage/profiling
GCOV_FLAGS := --coverage
PORT ?= 5555

.PHONY: all clean run gprof valgrind-memcheck coverage html kill-port

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INC) $(SRC) -o $@ $(LDFLAGS)

# Run
run: all
	./$(TARGET) $(ARGS)

# Clean build and debug artifacts
clean:
	rm -f $(TARGET) *.gcda *.gcno *.info gmon.out callgrind.out.* gprof_report.txt
	rm -rf html coverage

# GProf profiling
gprof: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -pg" LDFLAGS="$(LDFLAGS) -pg"
	./$(TARGET)
	gprof $(TARGET) gmon.out > gprof_report.txt

# Valgrind memory check
valgrind-memcheck: all
	valgrind --leak-check=full --track-origins=yes ./$(TARGET) $(ARGS)

# Code coverage (LCOV)
coverage: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) $(GCOV_FLAGS)" LDFLAGS="$(LDFLAGS) $(GCOV_FLAGS)"
	./$(TARGET)
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory html

# Optional: kill server on port (for later stages)
kill-port:
	-fuser -k $(PORT)/tcp 2>/dev/null || true

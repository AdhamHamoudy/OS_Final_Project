# Compiler and flags
CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -g -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnull-dereference -Wformat=2 -Wundef -Wpointer-arith -pthread
LDFLAGS := -pthread

# Directories
BIN := bin
TARGET := $(BIN)/euler_app
PORT ?= 5555

# Source files per stage
SRC := ../Stage1/graph.cpp ../Stage2/euler.cpp ../Stage3/main.cpp
INCLUDES := -I../Stage1 -I../Stage2 -I../Stage3

# Default target
.PHONY: all run clean gprof profile valgrind-memcheck valgrind-helgrind callgrind coverage server run-server kill-port

all: $(TARGET)

$(BIN):
	mkdir -p $(BIN)

$(TARGET): $(SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $@ $(LDFLAGS)

run: all
	$(TARGET)

clean:
	rm -rf $(BIN) *.gcda *.gcno *.info coverage html gmon.out callgrind.out.* gprof_report.txt

# ----- gprof profiling -----
gprof: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -pg" LDFLAGS="$(LDFLAGS) -pg" all
	$(TARGET) -n 6 -m 8 -s 42
	gprof $(TARGET) gmon.out > gprof_report.txt && echo "gprof_report.txt generated"

# ----- Valgrind tools -----
valgrind-memcheck: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TARGET) -n 6 -m 8 -s 42

valgrind-helgrind: all
	valgrind --tool=helgrind $(TARGET) -n 6 -m 8 -s 42

callgrind: all
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out.$(notdir $(TARGET)) $(TARGET) -n 6 -m 8 -s 42
	echo "Callgrind output: callgrind.out.$(notdir $(TARGET))"

# ----- Coverage report -----
coverage: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -fprofile-arcs -ftest-coverage -O0" LDFLAGS="$(LDFLAGS) -fprofile-arcs -ftest-coverage" all
	$(TARGET) -n 6 -m 8 -s 42 || true
	$(TARGET) -f ../g_ok5.txt || true
	$(TARGET) -n 4 -m 4 -s 7 || true
	$(TARGET) -n 3 -m 2 -s 1 || true
	$(TARGET) -n 6 -m 0 -s 42 || true
	mkdir -p coverage
	lcov --capture --directory . --base-directory .. \
		--include "$(abspath ../Stage1)/*" \
		--include "$(abspath ../Stage2)/*" \
		--include "$(abspath ../Stage3)/*" \
		--rc geninfo_unexecuted_blocks=1 \
		--output-file coverage/coverage.info
	genhtml coverage/coverage.info --output-directory coverage/html
	@echo "Coverage report available at coverage/html/index.html"

# ----- Server target -----
server:
	$(CXX) $(CXXFLAGS) -I../Stage1 -I../Stage2 -I../Stage4 ../Stage4/euler_server.cpp ../Stage1/graph.cpp ../Stage2/euler.cpp -o $(BIN)/euler_server $(LDFLAGS)

run-server: server
	./bin/euler_server $(PORT)

# ----- Utility -----
kill-port:
	@echo "Killing process using TCP port $(PORT)..."
	@fuser -k $(PORT)/tcp || true
	
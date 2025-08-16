CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -g -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnull-dereference -Wformat=2 -Wundef -Wpointer-arith -pthread
LDFLAGS := -pthread

INC := -Iinclude
SRC := $(wildcard src/*.cpp)
BIN := bin
TARGET := $(BIN)/euler_app

PORT ?= 5555

.PHONY: all run clean gprof profile valgrind-memcheck valgrind-helgrind callgrind coverage server kill-port

all: $(TARGET)

$(BIN):
	mkdir -p $(BIN)

$(TARGET): $(SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $(INC) $(SRC) -o $@ $(LDFLAGS)

run: all
	$(TARGET)

clean:
	rm -rf $(BIN) *.gcda *.gcno *.info coverage html gmon.out callgrind.out.* gprof_report.txt

# ----- gprof -----
gprof: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -pg" LDFLAGS="$(LDFLAGS) -pg" all
	$(TARGET) -n 6 -m 8 -s 42
	gprof $(TARGET) gmon.out > gprof_report.txt && echo "gprof_report.txt generated"

# ----- Valgrind -----
valgrind-memcheck: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TARGET) -n 6 -m 8 -s 42

valgrind-helgrind: all
	valgrind --tool=helgrind $(TARGET) -n 6 -m 8 -s 42

callgrind: all
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out.$(notdir $(TARGET)) $(TARGET) -n 6 -m 8 -s 42
	echo "Callgrind output: callgrind.out.$(notdir $(TARGET))"

# ----- Coverage -----
coverage: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -fprofile-arcs -ftest-coverage -O0" LDFLAGS="$(LDFLAGS) -fprofile-arcs -ftest-coverage" all
	$(TARGET) -n 6 -m 8 -s 42
	mkdir -p coverage
	lcov --capture --directory . --output-file coverage/coverage.info
	lcov --remove coverage/coverage.info '/usr/*' --output-file coverage/coverage.info
	genhtml coverage/coverage.info --output-directory coverage/html
	@echo "Open coverage/html/index.html"

# ----- Euler Server -----
server: $(BIN) src/euler_server.cpp src/graph.cpp src/euler.cpp
	$(CXX) $(CXXFLAGS) -Iinclude src/euler_server.cpp src/graph.cpp src/euler.cpp -o $(BIN)/euler_server $(LDFLAGS)

run-server: server
	./bin/euler_server $(PORT)

# ----- Utility -----
kill-port:
	@echo "Killing process using TCP port $(PORT)..."
	@fuser -k $(PORT)/tcp || true

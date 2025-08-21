# Makefile for Stage4

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
OPTFLAGS = -O2
DBGFLAGS = -O0 -g
COVFLAGS = --coverage
INCLUDES = -I../Stage1 -I../Stage2

SOURCES = ../Stage1/graph.cpp ../Stage2/euler.cpp ../Stage3/main.cpp
TARGET = euler_app
TARGET_COV = euler_app_cov
TARGET_DBG = euler_app_dbg
TARGET_GPROF = euler_app_gprof

ROOT := $(abspath ..)

.PHONY: all clean coverage

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)

$(TARGET_COV): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(COVFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET_COV)

$(TARGET_DBG): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET_DBG)

$(TARGET_GPROF): $(SOURCES)
	$(CXX) $(CXXFLAGS) -pg $(INCLUDES) $(SOURCES) -o $(TARGET_GPROF)

coverage: $(TARGET_COV)
	./$(TARGET_COV) -f ../g_ok5.txt || true
	./$(TARGET_COV) -n 4 -m 4 -s 7 || true
	./$(TARGET_COV) -n 3 -m 2 -s 1 || true
	./$(TARGET_COV) -n 6 -m 0 -s 42 || true

	mkdir -p coverage

	lcov --capture --directory . --base-directory /home/adham_lt_ubunto/OS_Final_Project \
     --include "/home/adham_lt_ubunto/OS_Final_Project/Stage1/" \
     --include "/home/adham_lt_ubunto/OS_Final_Project/Stage2/" \
     --include "/home/adham_lt_ubunto/OS_Final_Project/Stage3/" \
     --rc geninfo_unexecuted_blocks=1 \
     --output-file coverage/coverage.info

	genhtml coverage/coverage.info --output-directory coverage

clean:
	rm -f $(TARGET) $(TARGET_COV) $(TARGET_DBG) $(TARGET_GPROF)
	rm -f *.gcda *.gcno gmon.out
	rm -rf coverage reports

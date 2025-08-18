CXX ?= g++
INCS := -I../Stage1 -I../Stage2
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 $(INCS)

BIN := euler_app
SRCS := ../Stage1/graph.cpp ../Stage2/euler.cpp ../Stage3/main.cpp

all: $(BIN)
$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN)

# Optional: regenerate coverage/gprof/memcheck/callgrind from here
coverage:
	$(CXX) -std=c++17 -O0 -g --coverage $(INCS) $(SRCS) -o euler_app_cov
	./euler_app_cov -f ../g_ok5.txt || true
	lcov --capture --directory . --base-directory .. --no-external \
	     --rc geninfo_unexecuted_blocks=1 --output-file coverage.info
	genhtml coverage.info --output-directory coverage_html

gprof:
	$(CXX) -std=c++17 -O2 -pg $(INCS) $(SRCS) -o euler_app_gprof
	./euler_app_gprof -f ../g_ok5.txt
	gprof ./euler_app_gprof gmon.out > gprof_report.txt

memcheck:
	$(CXX) -std=c++17 -O2 -g $(INCS) $(SRCS) -o euler_app_dbg
	valgrind --leak-check=full --show-leak-kinds=all \
	  --log-file=valgrind_memcheck_file.txt \
	  ./euler_app_dbg -f ../g_ok5.txt

callgrind:
	$(CXX) -std=c++17 -O2 -g $(INCS) $(SRCS) -o euler_app_dbg
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out.euler \
	  ./euler_app_dbg -f ../g_ok5.txt

clean:
	rm -f $(BIN) euler_app_cov euler_app_gprof euler_app_dbg gmon.out \
	      *.gcda *.gcno coverage.info
	rm -rf coverage_html

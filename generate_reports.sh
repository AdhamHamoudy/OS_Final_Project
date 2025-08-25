#!/usr/bin/env bash
set -euo pipefail

# Clean + normal build (O2, debug info)
make clean
make

mkdir -p reports

echo "[A] Valgrind memcheck (file and random)"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
  ./bin/euler_app -i g_ok6.txt 2> reports/valgrind_memcheck_file.txt
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
  ./bin/euler_app -n 6 -m 8 -s 42 2> reports/valgrind_memcheck_random.txt

echo "[B] Helgrind (no threads yet; should be clean)"
valgrind --tool=helgrind ./bin/euler_app -i g_ok6.txt 2> reports/helgrind_file.txt

echo "[C] Callgrind (profiling call graph)"
valgrind --tool=callgrind --callgrind-out-file=reports/callgrind.out.euler \
  ./bin/euler_app -i g_ok6.txt
# Optional: kcachegrind reports/callgrind.out.euler

echo "[D] gprof (rebuild with -pg)"
make clean
make CXXFLAGS="-std=gnu++17 -O2 -g -pg -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnull-dereference -Wformat=2 -Wundef -Wpointer-arith -pthread" \
     LDFLAGS="-pg -pthread"
./bin/euler_app -i g_ok6.txt > /dev/null
gprof ./bin/euler_app gmon.out > reports/gprof_report.txt

echo "[E] Coverage (rebuild with gcov/lcov flags)"
make clean
make CXXFLAGS="-std=gnu++17 -O0 -g -fprofile-arcs -ftest-coverage -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnull-dereference -Wformat=2 -Wundef -Wpointer-arith -pthread" \
     LDFLAGS="-fprofile-arcs -ftest-coverage -pthread"

# Exercise multiple paths
./bin/euler_app -i g_ok5.txt > /dev/null
./bin/euler_app -i g_ok6.txt > /dev/null
./bin/euler_app -i g_trail_only.txt > /dev/null
./bin/euler_app -i g_disconnected.txt > /dev/null
./bin/euler_app -n 6 -m 8 -s 42 > /dev/null || true

# Collect LCOV
mkdir -p coverage
lcov --capture --directory . --output-file coverage/coverage.info
lcov --remove coverage/coverage.info '/usr/*' --output-file coverage/coverage.info
genhtml coverage/coverage.info --output-directory coverage/html

# Copy key files into reports/
cp -r coverage/html reports/coverage_html
echo "[OK] Reports are in ./reports"

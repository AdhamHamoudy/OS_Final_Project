# ---- Config ----
CXX      := g++
CXXFLAGS := -std=gnu++17 -g -O0 -Wall -Wextra -Wpedantic
VALGRIND := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

# Binaries
BAD_BIN   := buggy_bad
GOOD_BIN  := buggy_fixed

# Logs
BAD_LOG   := memcheck_bad.txt
GOOD_LOG  := memcheck_clean.txt

# ---- Default ----
.PHONY: all
all: $(BAD_BIN) $(GOOD_BIN)

# ---- Build ----
$(BAD_BIN): buggy.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(GOOD_BIN): buggy_fixed.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# ---- Memcheck runs ----
.PHONY: memcheck memcheck_bad memcheck_clean
memcheck: memcheck_bad memcheck_clean

memcheck_bad: $(BAD_BIN)
	$(VALGRIND) ./$(BAD_BIN) 2> $(BAD_LOG) || true
	@echo "Wrote $(BAD_LOG)"

memcheck_clean: $(GOOD_BIN)
	$(VALGRIND) ./$(GOOD_BIN) 2> $(GOOD_LOG)
	@echo "Wrote $(GOOD_LOG)"

# ---- Quick verification helpers (optional) ----
.PHONY: verify
verify: memcheck
	@echo "---- BAD run summary ----"
	@grep -E 'Invalid|lost|ERROR SUMMARY' -n $(BAD_LOG) || true
	@echo "---- CLEAN run summary ----"
	@grep -E 'All heap blocks were freed|ERROR SUMMARY' -n $(GOOD_LOG) || true

# ---- Cleanup ----
.PHONY: clean
clean:
	rm -f $(BAD_BIN) $(GOOD_BIN) $(BAD_LOG) $(GOOD_LOG)

# Step 5: Valgrind/memcheck analysis

## Build
g++ -g -O0 buggy.cpp -o buggy

## Command
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./buggy

## Findings (from memcheck.txt)
- Invalid write of size 4 at buggy.cpp:...
- Invalid write of size 1 at buggy.cpp:...
- Invalid free / double free at buggy.cpp:...
- 8 bytes in 1 blocks are definitely lost

## Root causes
1) Off-by-one loop writes to a[n] (index n). Must loop i < n.
2) strcpy into an 8-byte buffer needs 9 bytes including '\0'.
3) Double free on pointer b.
4) Leak of s (never freed).

## Fix (minimal changes)
- for (int i = 0; i < n; ++i) a[i] = i;
- char* s = (char*)std::malloc(9);  // or use strncpy
- Remove the second free(b);
- Add free(s);

## Clean re-run
Valgrind after fixes: "All heap blocks were freed — no leaks are possible", "ERROR SUMMARY: 0 errors".

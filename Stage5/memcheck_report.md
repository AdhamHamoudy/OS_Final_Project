# Stage 5 – Valgrind/Memcheck Analysis and Fixes

In this stage, we analyze the provided buggy program with **Valgrind/memcheck**, identify memory errors, and then fix them. Both the buggy and corrected versions are included, along with the memcheck logs.

---

## 1. Buggy Program (`buggy.cpp`)

### Errors Found by Valgrind
1. **Off-by-one write**  
   - Code:  
     ```cpp
     for (int i = 0; i <= n; ++i) a[i] = i;
     ```  
   - Issue: Loop writes to index `n` when only `0..n-1` are valid.  
   - Valgrind: *Invalid write of size 4*.  

2. **String overflow**  
   - Code:  
     ```cpp
     char* s = (char*)std::malloc(8);
     std::strcpy(s, "ABCDEFGH");
     ```  
   - Issue: `"ABCDEFGH"` requires 9 bytes (8 chars + `'\0'`), but only 8 were allocated.  
   - Valgrind: *Invalid write of size 1*.  

3. **Double free**  
   - Code:  
     ```cpp
     std::free(b);
     std::free(b);
     ```  
   - Issue: Memory freed twice.  
   - Valgrind: *Invalid free()*.

4. **Memory leak**  
   - Code:  
     ```cpp
     // s was never freed
     ```  
   - Issue: Leaked 8 bytes.  
   - Valgrind: *definitely lost: 8 bytes in 1 blocks*.  

### Memcheck Result
- File: `memcheck_bad.txt`  
- **Error summary:** 4 errors (invalid writes, double free, and leak).  

---

## 2. Fixed Program (`buggy_fixed.cpp`)

### Fixes Applied
1. **Fixed loop bound**  
   ```cpp
   for (int i = 0; i < n; ++i) a[i] = i;

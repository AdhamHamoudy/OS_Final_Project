#include <cstdlib>
#include <cstring>
#include <iostream>

int main() {
    int n = 10;

    // FIX 1: correct loop bound
    int* a = (int*)std::malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) a[i] = i;

    // FIX 2: allocate enough space for string + '\0'
    char* s = (char*)std::malloc(9);
    std::strcpy(s, "ABCDEFGH");

    std::free(a);

    // FIX 3: only free once
    int* b = (int*)std::malloc(4);
    std::free(b);

    // FIX 4: free string before exit
    std::free(s);

    return 0;
}

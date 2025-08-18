#include <cstdlib>
#include <cstring>
#include <iostream>

int main() {
    int n = 10;

    // BUG 1: off-by-one write (i <= n instead of i < n)
    int* a = (int*)std::malloc(n * sizeof(int));
    for (int i = 0; i <= n; ++i) a[i] = i;

    // BUG 2: too small allocation (needs 9 bytes incl '\0')
    char* s = (char*)std::malloc(8);
    std::strcpy(s, "ABCDEFGH"); // writes 9 bytes

    std::free(a);

    // BUG 3: double free
    int* b = (int*)std::malloc(4);
    std::free(b);
    std::free(b);

    // BUG 4: leak (s never freed)
    return 0;
}

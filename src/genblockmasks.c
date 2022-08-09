#include <stdint.h>
#include <stdio.h>

int diff(int x, int y) {
    return (x > y) ? y - x : x - y;
}

uint64_t run(int x) {
    
    int o = x & 63;
    int d = x >> 6;

    int diffx = diff(o % 8, d % 8);
    int diffy = diff(o / 8, d / 8);
    
    if (diffx == 0 && diffy == 0) return 0;
    if (diffx == diffy) {
        // diagonal
        uint64_t mask = 0;
        int dx = (o % 8 < d % 8) ? 1 : -1;
        int dy = (o / 8 < d / 8) ? 1 : -1;
        int sq = o + 8*dy + dx;
        while (sq != d) {
            mask += (1ULL << sq);
            sq += 8*dy + dx;
        }
        return mask;
    }
    if (diffx == 0) {
        uint64_t mask = 0;
        int dy = (o / 8 < d / 8) ? 1 : -1;
        int sq = o + 8*dy;
        while (sq != d) {
            mask += (1ULL << sq);
            sq += 8*dy;
        }
        return mask;
    }
    else if (diffy == 0) {
        uint64_t mask = 0;
        int dx = (o % 8 < d % 8) ? 1 : -1;
        int sq = o + dx;
        while (sq != d) {
            mask += (1ULL << sq);
            sq += dx;
        }
        return mask;
    }
    return 0;
}

int main() {
    printf("{");
    for (int i = 0; i < 4096; i++) {
       uint64_t mask = run(i);
       if (mask) printf("%luULL,", mask);
       else printf("%lu,",mask);
       if ((i+1)%16 == 0) printf("\n");

    }
    printf("}\n");
}
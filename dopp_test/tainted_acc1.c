#include <sys/types.h>
#include <malloc.h>
#include <stdlib.h>

#define LINESIZE 128
#define NLINES 127

void flush(void* addr){
    asm volatile("clflush (%0)" : : "r"(addr));
}

int main(int argc, char** argv){
    size_t n_iters = 1000000;
    if (argc > 1) {
        n_iters = atoi(argv[1]);
    }

    register char* a = (char*)malloc(LINESIZE * NLINES);
    register char* b = (char*)malloc(LINESIZE * NLINES);

    for (size_t i = 0; i < LINESIZE * NLINES; i++) {
        a[i] = 71;
        b[i] = 123;
    }

    char acc = 1;
    register unsigned long y = 1;

    for (register size_t i = 0; i < n_iters/(NLINES) + 1; i++) {
        for (register size_t j = 0; j < NLINES; j++) {
            flush(&acc);
            if (acc){
                register size_t idx = LINESIZE * b[j];
                y += a[idx];
            }
        }
    }

    printf("y: %lu\n", y);
}

#include <sys/types.h>
#include <malloc.h>
#include <stdlib.h>

#define PAGE_SIZE 4096
#define NPAGES 19

void flush(void* addr){
    // asm volatile("mfence\n clflush (%0)\n mfence" : : "r"(addr));
    asm volatile("clflush (%0)" : : "r"(addr));
}

int main(int argc, char** argv){
    size_t n_iters = 1000000;
    if (argc > 1) {
        n_iters = atoi(argv[1]);
    }

    char* a = (char*)malloc(PAGE_SIZE * NPAGES);
    char* b = (char*)malloc(PAGE_SIZE * NPAGES);

    for (size_t i = 0; i < PAGE_SIZE * NPAGES; i++) {
        a[i] = 234;
        b[i] = i % NPAGES;
    }

    char acc = 1;
    size_t y = 1;

    for (register size_t i = 0; i < n_iters/(NPAGES * PAGE_SIZE); i++) {
        for (register size_t j = 0; j < NPAGES * PAGE_SIZE; j++) {
            if (acc){
                register size_t idx = NPAGES * b[j];
                y += a[idx];
                // flush(&a[idx]);
            }
            flush(&acc);
        }
    }

    printf("y: %zu\n", y);
}
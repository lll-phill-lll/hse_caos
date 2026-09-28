#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

void check_args(int argc, const char *program) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <binary> <a> <b>\n", program);
        exit(1);
    }
}

size_t get_page_size() {
    long size = sysconf(_SC_PAGESIZE);

    if (size == -1) {
        perror("sysconf");
        exit(1);
    }

    return size;
}

unsigned char *allocate_rw_memory(size_t size) {
    unsigned char *memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    return memory;
}

size_t load_binary(const char *filename, unsigned char *memory, size_t capacity) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    size_t size = fread(memory, 1, capacity, file);

    if (ferror(file)) {
        perror("fread");
        exit(1);
    }

    fclose(file);

    return size;
}

void print_bytes(const unsigned char *memory, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%02x ", memory[i]);
    }

    printf("\n");
}

void make_executable(void *memory, size_t size) {
    if (mprotect(memory, size, PROT_READ | PROT_EXEC) == -1) {
        perror("mprotect");
        exit(1);
    }
}

int main(int argc, char **argv) {
    check_args(argc, argv[0]);

    const char *filename = argv[1];
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);

    size_t page_size = get_page_size();

    unsigned char *code = allocate_rw_memory(page_size);

    size_t code_size = load_binary(filename, code, page_size);

    print_bytes(code, code_size);

    make_executable(code, page_size);

    typedef int (*BinaryFunction)(int, int);
    BinaryFunction function = (BinaryFunction)code;

    int result = function(a, b);

    printf("minmax(%d, %d) = %d\n", a, b, result);

    munmap(code, page_size);
}

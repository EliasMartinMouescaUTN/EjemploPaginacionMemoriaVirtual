#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>

char constant[] = "constante";      // Global inicializada (.data)
char* literal = "string literal";   // Read only (.rodata)
int global;                         // Global no inicializada (.bss)

int pageSizeExponent;
int pageSize;

bool hasPermission = false;

void printPointer(const char* str, void* ptr);
void printHumanSize(uint64_t bytes);
uint64_t getFrame(void* vaddr);

int main(int argc, char* argv[]) {
//int main() {
    pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize == -1) {
        printf("sysconf() retornó '%s'\n", strerror(errno));
        return 1;
    }

    pageSizeExponent = __builtin_ctz(pageSize);

    printf("Tamaño de página: %d (2 ^ %d)\n\n", pageSize, pageSizeExponent);

    int variableStack;
    void* stackPointer = (void*) &variableStack;    // Stack
    void* heapPointer = malloc(sizeof(int));        // Heap

    printPointer("Variable en el stack      ", stackPointer);
    printPointer("Función de libc           ", (void*)printf);
    printPointer("Variable en la heap       ", heapPointer);
    printPointer("Global inicializada       ", (void*)constant);
    printPointer("Global no inicializada    ", (void*)&global);
    printPointer("Cadena literal            ", (void*)literal);
    printPointer("Función del programa      ", (void*)printPointer);

    free(heapPointer);

    if (!hasPermission)
        puts("[El programa no obtuvo los permisos requeridos para mostrar los frames físicos.]");

    if (argc > 1)
        if (strcmp(argv[1], "--wait") == 0) {
            puts("\n\nEsperando... (los números de frame no se están actualizando)");
            getchar();
        }

    return 0;
}

void printPointer(const char* str, void* ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t page = addr >> pageSizeExponent;

    printf("%s:\t%p, página %" PRIuPTR, str, ptr, page);

    uint64_t pfn = getFrame(ptr);
    if (pfn) {
        printf(" → frame %" PRIu64, pfn);
        hasPermission = true;
    }

    printf("\n");
}

void printHumanSize(uint64_t bytes) {
    const char *units[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB"};
    double size = (double)bytes;
    int unit = 0;

    while (size >= 1024.0 && unit < 5) {
        size /= 1024.0;
        unit++;
    }
    printf("%.2f %s", size, units[unit]);
}

uint64_t getFrame(void* vaddr) {
    uint64_t entry;
    ssize_t nread;
    off_t offset;
    int fd;

    fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    offset = ((uintptr_t)vaddr / pageSize) * sizeof(entry);
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        close(fd);
        return 0;
    }

    nread = read(fd, &entry, sizeof(entry));
    close(fd);

    if (nread != sizeof(entry))
        return 0;

    if (!(entry & (1ULL << 63)))
        return 0;


    return entry & ((1ULL << 55) - 1);
}


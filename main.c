#include <unistd.h>
#include <locale.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>

char constant[] = "constante";      // Global inicializada (.data)
char* literal = "string literal";   // Read only (.rodata)
int global;                         // Global no inicializada (.bss)

int pageSizeExponent;
long pages;
long pageSize;

bool hasPermission = false;

void printPointer(const char* str, void* ptr);
void printHumanSize(uint64_t bytes);
uint64_t getFrame(void* vaddr);

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "es_AR.UTF-8");

    pages = sysconf(_SC_PHYS_PAGES);
    if (pages == -1) {
        printf("sysconf() retornó '%s'\n", strerror(errno));
        return 1;
    }

    pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize == -1) {
        printf("sysconf() retornó '%s'\n", strerror(errno));
        return 1;
    }

    pageSizeExponent = __builtin_ctz(pageSize);

    printf("La memoria física está dividida en %'ld frames de %'ld (2^%d) bytes cada uno.\n\n", pages, pageSize, pageSizeExponent);

    int variableStack;
    void* heapAddress = malloc(sizeof(int));        // Heap
    int fd = open("Makefile", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    size_t size = st.st_size;
    void *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);
    volatile char c = *((char*) map);    // Leemos algo para que el SO cargue el archivo a memoria



    printPointer("Variable en el stack      ", (void*) &variableStack);
    puts("(...)");
    printPointer("Archivo mapeado a memoria ", map);
    printPointer("Función de shared library ", (void*)printf);
    puts("(...)");
    printPointer("Variable en la heap       ", heapAddress);
    printPointer("Global inicializada       ", (void*)constant);
    printPointer("Global no inicializada    ", (void*)&global);
    printPointer("Cadena literal            ", (void*)literal);
    printPointer("Función del programa      ", (void*)printPointer);

    free(heapAddress);
    munmap(map, size);

    if (!hasPermission)
        puts("\n[ El programa no obtuvo los permisos requeridos para mostrar los frames físicos. ]");

    if (argc > 1)
        if (strcmp(argv[1], "--wait") == 0 || strcmp(argv[1], "-w") == 0) {
            //puts("\n\nEsperando... (los números de frame no se están actualizando)");
            getchar();
        }

    return 0;
}

void printPointer(const char* str, void* ptr) {
    uintptr_t addr   = (uintptr_t) ptr;
    uintptr_t offset = addr & (pageSize - 1);
    uintptr_t base   = addr - offset;  // start of the page
    uintptr_t page   = addr >> pageSizeExponent;

    printf("%s:\t%p (%p + 0x%lx)\t\tpágina %'" PRIuPTR, 
           str, ptr, (void*) base, (unsigned long) offset, page);

    uint64_t pfn = getFrame(ptr);
    if (pfn) {
        printf("  ->  frame %'" PRIu64, pfn);
        hasPermission = true;
    }
    puts("");
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


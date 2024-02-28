#include "brc.h"
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

static const size_t THREADS = 20;

void brcFunc(StationManager *manager, char *file, size_t size) {
    char *from = file;
    std::string_view name, temp;

    for (size_t i = 0; i < size; i++) {
        if (file[i] == ';') {
            name = std::string_view(from, &file[i] - from);
            from = &file[i + 1];
        } else if (file[i] == '\n') {
            temp = std::string_view(from, &file[i] - from);
            from = &file[i + 1];
            manager->add(name, tempFromString(temp));
        }
    }
}

char *calcRealStart(char *file, size_t size, size_t offset) {
    for (; offset < size; offset++) {
        if (file[offset] == '\n') {
            return &file[offset + 1];
        }
    }
    throw std::runtime_error("missing '\n' perhaps?");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Enter filename" << std::endl;
        return 0;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        std::cout << "Failed to open " << argv[1] << std::endl;
        return errno;
    }
    struct stat stats;
    int status = fstat(fd, &stats);
    if (status < 0) {
        std::cout << "Failed to get fstat" << std::endl;
        return errno;
    }
    size_t size = stats.st_size;
    char *file = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (intptr_t(file) < 0) {
        std::cout << "Failed to mmap" << std::endl;
        return errno;
    }

    size_t offset = 0;
    std::vector<char *> startPtrs = {file};
    char *temp = file;
    for (size_t i = 1; i < THREADS; i++) {
        offset += size / THREADS;
        startPtrs.push_back(calcRealStart(file, size, offset));
    }
    startPtrs.push_back(&file[size]);

    std::thread threads[THREADS];
    StationManager managers[THREADS];

    for (size_t i = 0; i < startPtrs.size() - 1; i++) {
        size_t partial = startPtrs[i + 1] - startPtrs[i];
        threads[i] = std::thread(brcFunc, &managers[i], startPtrs[i], partial);
    }

    StationManager combined;

    for (size_t i = 0; i < THREADS; i++) {
        threads[i].join();
        combined.add(managers[i]);
    }

    std::cout << combined << std::endl;
    munmap(file, size);
    return 0;
}

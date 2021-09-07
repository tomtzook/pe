
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"
#include "debug.h"


void readFile(const char* path, std::vector<char>& buffer) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer.resize(fileSize);
    file.read(buffer.data(), fileSize);
}

int main(int argc, char** argv) {
    const char* pePath = argv[1];

    std::vector<char> buffer;
    readFile(pePath, buffer);

    pe::Image image(buffer.data());
    std::cout << image << std::endl;

    for (const auto& section : image.sections()) {
       std::cout << section << std::endl;
    }

    return 0;
}


#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"

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
    const char* exportName = argv[2];

    std::vector<char> buffer;
    readFile(pePath, buffer);

    pe::Image image(buffer.data());

    auto peExport = image.exportSection();
    pe::rva_t rva = peExport.findExportByName(exportName);
    size_t offset = image.rvaToOffset(rva);
    auto ptr = image.rvaToPointer<void>(rva);

    printf("Found export %s at RVA=0x%x, OFFSET=0x%zx, RAW_PTR=0x%p\n",
           exportName, rva, offset, ptr);

    return 0;
}

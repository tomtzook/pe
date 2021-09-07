
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"
#include "debug.h"
#include "except.h"


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
    std::cout << image << std::endl;

    try {
        auto exportTable = image.exportTable();

        auto entry = exportTable.names()[exportName];
        pe::rva_t rva = exportTable[entry.ordinal()];

        size_t offset = image.rvaToOffset(rva);
        auto ptr = image.rvaToPointer<void>(rva);

        std::cout << "Found export " << exportName <<
                  " at RVA=0x" << std::hex << rva <<
                  ", OFFSET=0x" << std::hex << offset <<
                  ", PTR=0x" << reinterpret_cast<const void*>(ptr) <<
                  std::endl;
    } catch (const pe::NameNotFoundException&) {
        std::cout << "Export " << exportName << " not found" << std::endl;
    } catch (const pe::NoExportTableException&) {
        std::cout << "PE does not have export table" << std::endl;
    }

    return 0;
}

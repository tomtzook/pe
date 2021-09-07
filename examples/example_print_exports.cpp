
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"
#include "except.h"
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

    try {
        auto exportTable = image.exportTable();

        std::cout << "EXPORT TABLE" << std::endl;
        for(const auto& addressEntry : exportTable) {
            std::cout << " rva=0x" << std::hex << addressEntry.rva()
                      << ", ordinal=0x" << std::hex << addressEntry.ordinal()
                      << ", baised=0x" << std::hex << addressEntry.baisedOrdinal()
                      << ", ptr=0x" << reinterpret_cast<const void*>(image.rvaToPointer<uint8_t>(addressEntry.rva()));

            try {
                auto nameEntry = exportTable.names()[addressEntry.ordinal()];
                std::cout << " name=" << nameEntry.name();
            } catch (const pe::OrdinalNotFoundException& ex) {
                // no name for export
            }

            std::cout << std::endl;
        }
    } catch (const pe::NoExportTableException& ex) {
        std::cout << "PE does not have export table" << std::endl;
    }

    return 0;
}

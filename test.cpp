
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"
#include "except.h"
#include "debug.h"


int main() {
    std::ifstream file("/home/tomtzook/vm/shared/ntoskrnl.exe", std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    try {
        pe::Image image(buffer.data());
        std::cout << image << std::endl;

        for (const auto& section : image.sections()) {
            std::cout << section << std::endl;
        }

        {
            auto peExport = image.exportSection();
            auto entry = peExport.names()["ExAllocatePoolWithTag"];
            pe::rva_t rva = peExport[entry.ordinal()];
            auto ptr = image.rvaToPointer<uint8_t>(rva);

            std::cout << entry << " ptr=" << reinterpret_cast<const void*>(ptr) << std::endl;
        }

    } catch (const pe::BadHeaderException& ex) {
        printf("Bad Header: %d\n", ex.reason());
    }

    return 0;
}

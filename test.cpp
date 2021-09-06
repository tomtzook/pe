
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

        for (auto& section : image.sections()) {
            printf("Section: %s\n", section.name());
        }

        {
            auto peExport = image.exportSection();
            /*pe::rva_t rva = peExport.findExportByName("ExAllocatePoolWithTag");
            auto ptr = image.rvaToPointer<uint8_t>(rva);
            printf("Found: 0x%x\n", ptr);*/

            auto entry = peExport.names()["ExAllocatePoolWithTag"];
            pe::rva_t rva = peExport[entry.ordinal()];
            auto ptr = image.rvaToPointer<uint8_t>(rva);

            std::cout << entry << " ptr=" << reinterpret_cast<const void*>(ptr) << std::endl;

            /*for (const auto& entry : peExport.names()) {
                printf("f=%s, s=%d\n", entry.first, entry.second);
            }*/
        }

    } catch (const pe::BadHeaderException& ex) {
        printf("Bad Header: %d\n", ex.reason());
    }

    return 0;
}

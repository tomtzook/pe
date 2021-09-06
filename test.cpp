
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"
#include "except.h"


int main() {
    std::ifstream file("/home/tomtzook/vm/shared/ntoskrnl.exe", std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    try {
        pe::Image image(buffer.data());
        printf("IMAGE: size=0x%x, machine=0x%x, subsystem=0x%x\n",
               image.size(), image.machineType(), image.subsystem());

        for (auto& section : image.sections()) {
            printf("Section: %s\n", section.name());
        }

        {
            auto peExport = image.exportSection();
            pe::rva_t rva = peExport.findExportByName("MmAllocateContiguousMemory");
            auto ptr = image.rvaToPointer<uint8_t>(rva);

            printf("Found: 0x%x\n", ptr);
        }

    } catch (const pe::BadHeaderException& ex) {
        printf("Bad Header: %d\n", ex.reason());
    }

    return 0;
}

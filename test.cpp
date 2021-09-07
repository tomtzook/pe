
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"
#include "except.h"
#include "debug.h"


int main() {
    //"/home/tomtzook/vm/shared/jdk-11.0.11_windows-x64_bin.exe";//
    const char* path = "/home/tomtzook/vm/shared/jdk-11.0.11_windows-x64_bin.exe";//"/home/tomtzook/vm/shared/ntoskrnl.exe";
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    try {
        pe::Image image(buffer.data());
        std::cout << image << std::endl;

        for (const auto& module : image.importTable()) {
            printf("Module: %s\n", module.name());
            for (const auto& function : module) {
                printf("\tFunction: ");
                if (function.is_ordinal()) {
                    printf("ordinal=0x%x", function.ordinal());
                } else if (function.is_name()) {
                    printf("name=%s", function.name());
                } else {
                    printf("<n/a>");
                }
                printf("\n");
            }
        }

        for (const auto& section : image.sections()) {
            std::cout << section << std::endl;
        }

        {
            /*auto exportTable = image.exportTable();

            std::cout << "ADDRESS TABLE" << std::endl;
            for(const auto& addressEntry : exportTable) {
                std::cout << " rva=" << addressEntry.rva()
                          << ", ordinal=" << addressEntry.ordinal()
                          << ", baised=" << addressEntry.baisedOrdinal()
                          << ", ptr=" << reinterpret_cast<const void*>(image.rvaToPointer<uint8_t>(addressEntry.rva()));
                          //<< std::endl;

                try {
                    auto nameEntry = exportTable.names()[addressEntry.ordinal()];
                    std::cout << " name=" << nameEntry.name();
                } catch (const pe::OrdinalNotFoundException& ex) {
                    // no name for export
                }

                std::cout << std::endl;
            }*/

            /*std::cout << "NAME TABLE" << std::endl;
            for(const auto& aentry : exportTable.names()) {
                std::cout << " name=" << aentry.name()
                          << ", ordinal=" << aentry.ordinal()
                          << ", baised=" << aentry.baisedOrdinal()
                          << std::endl;
            }*/

            /*{
                auto entry = exportTable.names()["ExAllocatePoolWithTag"];
                pe::rva_t rva = exportTable[entry.ordinal()];
                auto ptr = image.rvaToPointer<uint8_t>(rva);
                std::cout << entry << " ptr=" << reinterpret_cast<const void*>(ptr) << std::endl;
            }

            {
                pe::export_ordinal_t ordinal = 0xa;
                pe::rva_t rva = exportTable[ordinal];
                auto entry = exportTable.names()[ordinal];
                auto ptr = image.rvaToPointer<uint8_t>(rva);
                std::cout << entry << " ptr=" << reinterpret_cast<const void*>(ptr) << std::endl;
            }*/
        }

    } catch (const pe::BadHeaderException& ex) {
        printf("Bad Header: %d\n", ex.reason());
    }

    return 0;
}

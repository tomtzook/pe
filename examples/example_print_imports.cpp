
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
        auto importTable = image.importTable();

        std::cout << "IMPORT TABLE" << std::endl;
        for(const auto& importedModule : importTable) {
            std::cout << "MODULE name=" << importedModule.name() << std::endl;
            for (const auto& entry : importedModule) {
                std::cout << "\tFUNCTION " << entry << std::endl;
            }
        }
    } catch (const pe::NoImportTableException&) {
        std::cout << "PE does not have import table" << std::endl;
    }

    return 0;
}

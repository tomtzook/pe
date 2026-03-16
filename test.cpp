
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"


int main() {
    const auto path = "/home/tomtzook/development/projects/hype/cmake-build-debug-clang/bin/hype.efi";
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::end);
    const size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(file_size);
    file.read(buffer.data(), file_size);

    pe::image image(buffer.data());
    /*for (const auto& section : image.sections()) {
        printf("section: %s\n", section.name());
    }

    auto function_table = image.load_exception_table();
    for (const auto& entry : function_table) {
        printf("Entry: 0x%x -> 0x%x [0x%x](%s)\n", entry.start(), entry.end(), entry.unwind_info_rva(), image.sections()[entry.unwind_info_rva()].name());

        auto info = pe::unwind_info(image.sections().rva_to_pointer<pe::UnwindInfo>(entry.unwind_info_rva()));
        do {
            printf("\tInfo: FR=0x%x FRO=0x%x, codes=%lu\n", info.frame_register(), info.frame_register_offset(), info.codes_count());
            info = info.next();
        } while (info.has_next());
    }*/

    return 0;
}

#pragma once

#include <cstddef>

#include "winnt_def.h"


namespace pe {

struct Section {
public:
    Section(const uint8_t* imageBase, const ImageNtHeaders64* ntHeaders,
            const ImageSectionHeader* header);

    const char* name() const;
    size_t alignedVirtualSize() const;

    bool containsRva(rva_t rva) const;
    size_t rvaToOffset(rva_t rva) const;

    template<typename T>
    const T* rvaToPointer(rva_t rva) const {
        size_t offset = rvaToOffset(rva);
        return reinterpret_cast<const T*>(m_base + offset);
    }

private:
    const uint8_t* m_base;
    const ImageNtHeaders64* m_ntHeaders;
    const ImageSectionHeader* m_header;
};

}

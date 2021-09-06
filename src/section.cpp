
#include "util.h"
#include "except.h"
#include "section.h"


namespace pe {

Section::Section(const uint8_t* imageBase, const ImageNtHeaders64* ntHeaders, const ImageSectionHeader* header)
    : m_base(imageBase)
    , m_ntHeaders(ntHeaders)
    , m_header(header)
{}

const char* Section::name() const {
    return reinterpret_cast<const char*>(m_header->Name);
}

size_t Section::alignedVirtualSize() const {
    if (0 != m_header->Misc.VirtualSize) {
        return roundUp(m_header->Misc.VirtualSize, m_ntHeaders->OptionalHeader.SectionAlignment);
    }

    return roundUp(m_header->SizeOfRawData, m_ntHeaders->OptionalHeader.SectionAlignment);
}

bool Section::containsRva(rva_t rva) const {
    return rva >= m_header->VirtualAddress &&
        rva <= m_header->VirtualAddress + alignedVirtualSize();
}

size_t Section::rvaToOffset(rva_t rva) const {
    if (!containsRva(rva)) {
        throw RvaNotInSectionException(*this, rva);
    }

    return m_header->PointerToRawData + rva - m_header->VirtualAddress;
}

}


#include <cstring>

#include "util.h"
#include "except.h"
#include "section.h"


namespace pe {

Section::Section(const PeHeaders& headers, const ImageSectionHeader* header)
    : m_headers(headers)
    , m_header(header)
{}

const char* Section::name() const {
    return reinterpret_cast<const char*>(m_header->Name);
}

size_t Section::alignedVirtualSize() const {
    if (0 != m_header->Misc.VirtualSize) {
        return roundUp(static_cast<size_t>(m_header->Misc.VirtualSize), m_headers.sectionAlignment());
    }

    return roundUp(static_cast<size_t>(m_header->SizeOfRawData), m_headers.sectionAlignment());
}

SectionCharacteristics Section::characteristics() const {
    return SectionCharacteristics {.data=m_header->Characteristics};
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

ImageSections::iterator::iterator(const PeHeaders& headers, const ImageSectionHeader* header)
        : m_headers(headers)
        , m_sectionHeader(header)
{}

ImageSections::iterator& ImageSections::iterator::operator++() {
    m_sectionHeader++;
    return *this;
}
ImageSections::iterator& ImageSections::iterator::operator--() {
    m_sectionHeader--;
    return *this;
}

ImageSections::iterator::reference ImageSections::iterator::operator*() {
    return {m_headers, m_sectionHeader};
}
ImageSections::iterator::pointer ImageSections::iterator::operator->() {
    return {m_headers, m_sectionHeader};
}

bool ImageSections::iterator::operator==(const iterator& rhs) {
    return m_sectionHeader == rhs.m_sectionHeader;
}
bool ImageSections::iterator::operator!=(const iterator& rhs) {
    return m_sectionHeader != rhs.m_sectionHeader;
}

ImageSections::ImageSections(const PeHeaders& headers)
        : m_headers(headers)
        , m_sectionHeaders(m_headers.getSectionHeaders())
{}

size_t ImageSections::count() const {
    return m_headers.sectionsCount();
}

Section ImageSections::operator[](const char* name) const {
    for(const auto& section : *this) {
        if (0 == strcmp(name, section.name())) {
            return section;
        }
    }

    throw SectionNotFoundException(name);
}

Section ImageSections::operator[](rva_t rva) const {
    for(const auto& section : *this) {
        if (section.containsRva(rva)) {
            return section;
        }
    }

    throw RvaNotInImageException(rva);
}

ImageSections::iterator ImageSections::begin() const {
    return iterator(m_headers, m_sectionHeaders);
}
ImageSections::iterator ImageSections::end() const {
    return iterator(m_headers, m_sectionHeaders + count());
}

}

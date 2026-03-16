
#include "util.h"
#include "section.h"


namespace pe {

section::section(const headers& headers, const ImageSectionHeader* header)
    : m_pe_headers(headers)
    , m_header(header)
{}

bool section::is_valid() const {
    return m_header != nullptr;
}

const char* section::name() const {
    return reinterpret_cast<const char*>(m_header->Name);
}

size_t section::aligned_virtual_size() const {
    if (0 != m_header->Misc.VirtualSize) {
        return round_up(static_cast<size_t>(m_header->Misc.VirtualSize), m_pe_headers.section_alignment());
    }

    return round_up(static_cast<size_t>(m_header->SizeOfRawData), m_pe_headers.section_alignment());
}

SectionCharacteristics section::characteristics() const {
    return SectionCharacteristics {.data=m_header->Characteristics};
}

bool section::contains_rva(const rva_t rva) const {
    return rva >= m_header->VirtualAddress &&
        rva <= m_header->VirtualAddress + aligned_virtual_size();
}

size_t section::rva_to_offset(const rva_t rva) const {
    if (!contains_rva(rva)) {
        return static_cast<size_t>(-1);
    }

    return m_header->PointerToRawData + rva - m_header->VirtualAddress;
}

section_list::iterator::iterator(const headers& headers, const ImageSectionHeader* header)
        : m_pe_headers(headers)
        , m_header(header)
{}

section_list::iterator& section_list::iterator::operator++() {
    m_header++;
    return *this;
}
section_list::iterator& section_list::iterator::operator--() {
    m_header--;
    return *this;
}

section_list::iterator::reference section_list::iterator::operator*() {
    return {m_pe_headers, m_header};
}
section_list::iterator::pointer section_list::iterator::operator->() {
    return {m_pe_headers, m_header};
}

bool section_list::iterator::operator==(const iterator& rhs) const {
    return m_header == rhs.m_header;
}
bool section_list::iterator::operator!=(const iterator& rhs) const {
    return m_header != rhs.m_header;
}

section_list::section_list(const headers& headers)
        : m_pe_headers(headers)
        , m_headers(m_pe_headers.section_headers())
{}

size_t section_list::count() const {
    return m_pe_headers.sections_count();
}

bool section_list::contains_rva(const rva_t rva) const {
    for (const auto& section : *this) {
        if (section.contains_rva(rva)) {
            return true;
        }
    }

    return false;
}

size_t section_list::rva_to_offset(const rva_t rva) const {
    for (const auto& section : *this) {
        if (section.contains_rva(rva)) {
            return section.rva_to_offset(rva);
        }
    }

    return false;
}

section section_list::operator[](const char* name) const {
    for (const auto& section : *this) {
        if (0 == __builtin_strcmp(name, section.name())) {
            return section;
        }
    }

    return {m_pe_headers, nullptr};
}

section section_list::operator[](const rva_t rva) const {
    for(const auto& section : *this) {
        if (section.contains_rva(rva)) {
            return section;
        }
    }

    return {m_pe_headers, nullptr};
}

section_list::iterator section_list::begin() const {
    return {m_pe_headers, m_headers};
}
section_list::iterator section_list::end() const {
    return {m_pe_headers, m_headers + count()};
}

}

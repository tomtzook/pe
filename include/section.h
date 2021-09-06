#pragma once

#include <cstddef>

#include "winnt_def.h"
#include "pe_base.h"


namespace pe {

struct Section {
public:
    Section(const PeHeaders& headers, const ImageSectionHeader* header);

    const char* name() const;
    size_t alignedVirtualSize() const;

    bool containsRva(rva_t rva) const;
    size_t rvaToOffset(rva_t rva) const;

    template<typename T>
    const T* rvaToPointer(rva_t rva) const {
        size_t offset = rvaToOffset(rva);
        return reinterpret_cast<const T*>(m_headers.base() + offset);
    }

private:
    const PeHeaders& m_headers;
    const ImageSectionHeader* m_header;
};

class ImageSections {
public:
    class iterator {
    public:
        using value_type = Section;
        using reference = Section;
        using pointer = Section;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(const PeHeaders& headers, const ImageSectionHeader* header);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs);
        bool operator!=(const iterator& rhs);

    private:
        const PeHeaders& m_headers;
        const ImageSectionHeader* m_sectionHeader;
    };

    explicit ImageSections(const PeHeaders& headers);

    size_t count() const;

    Section operator[](const char* name) const;

    iterator begin() const;
    iterator end() const;

private:
    const PeHeaders& m_headers;
    const ImageSectionHeader* m_sectionHeaders;
};

}

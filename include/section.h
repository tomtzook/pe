#pragma once

#ifndef PE_NO_STD
#include <iterator>
#endif

#include "winnt_def.h"
#include "pe_base.h"


namespace pe {

struct section {
    section(const headers& headers, const ImageSectionHeader* header);

    [[nodiscard]] bool is_valid() const;

    [[nodiscard]] const char* name() const;
    [[nodiscard]] uint32_t virtual_address() const;
    [[nodiscard]] size_t virtual_size() const;
    [[nodiscard]] size_t aligned_virtual_size() const;
    [[nodiscard]] uint32_t pointer_to_raw_data() const;
    [[nodiscard]] size_t size_of_raw_data() const;
    [[nodiscard]] SectionCharacteristics characteristics() const;

    [[nodiscard]] bool contains_rva(rva_t rva) const;
    [[nodiscard]] size_t rva_to_offset(rva_t rva) const;

    template<typename T>
    const T* rva_to_pointer(const rva_t rva) const {
        const auto offset = rva_to_offset(rva);
        return reinterpret_cast<const T*>(m_pe_headers.base() + offset);
    }

private:
    const headers& m_pe_headers;
    const ImageSectionHeader* m_header;
};

class section_list {
public:
    class iterator {
    public:
        using value_type = section;
        using reference = section;
        using pointer = section;

#ifndef PE_NO_STD
        using iterator_category = std::bidirectional_iterator_tag;
#endif


        iterator(const headers& headers, const ImageSectionHeader* header);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        const headers& m_pe_headers;
        const ImageSectionHeader* m_header;
    };

    explicit section_list(const headers& headers);

    [[nodiscard]] size_t count() const;
    [[nodiscard]] bool contains_rva(rva_t rva) const;
    [[nodiscard]] size_t rva_to_offset(rva_t rva) const;

    template<typename T>
    const T* rva_to_pointer(const rva_t rva) const {
        const auto offset = rva_to_offset(rva);
        return reinterpret_cast<const T*>(m_pe_headers.base() + offset);
    }

    section operator[](const char* name) const;
    section operator[](rva_t rva) const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

private:
    const headers& m_pe_headers;
    const ImageSectionHeader* m_headers;
};

}

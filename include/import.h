#pragma once

#ifndef PE_NO_STD
#include <iterator>
#endif

#include "winnt_def.h"
#include "section.h"

namespace pe {

class imported_module {
public:
    class iterator;

    struct entry {
        entry(const entry& other) = default;

        [[nodiscard]] bool is_ordinal() const;
        [[nodiscard]] uint16_t ordinal() const;

        [[nodiscard]] bool is_name() const;
        [[nodiscard]] const char* name() const;

    private:
        entry(const ImageThunkData64* thunk_data, const section& section);

        const ImageThunkData64* m_thunk_data;
        const section& m_section;

        friend iterator;
    };

    class iterator {
    public:
        using value_type = entry;
        using reference = entry;
        using pointer = entry;

#ifndef PE_NO_STD
        using iterator_category = std::bidirectional_iterator_tag;
#endif

        iterator(const ImageThunkData64* thunk_data, const section& section);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        const ImageThunkData64* m_thunk_data;
        const section& m_section;
    };

    imported_module(const ImageImportDescriptor* descriptor, const section& section);

    [[nodiscard]] const char* name() const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

private:
    [[nodiscard]] size_t find_last_data_index() const;

    const ImageImportDescriptor* m_descriptor;
    const section& m_section;
};

class import_table {
public:
    class iterator {
    public:
        using value_type = imported_module;
        using reference = imported_module;
        using pointer = imported_module;

#ifndef PE_NO_STD
        using iterator_category = std::bidirectional_iterator_tag;
#endif

        iterator(const ImageImportDescriptor* import_descriptor, const section& section);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        const ImageImportDescriptor* m_import_descriptor;
        const section& m_section;
    };

    import_table(const ImageImportDescriptor* import_descriptors, section section);

    [[nodiscard]] bool is_valid() const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

private:
    [[nodiscard]] size_t find_last_data_index() const;

    const ImageImportDescriptor* m_import_descriptors;
    section m_section;
};

}

#pragma once

#ifndef PE_NO_STD
#include <iterator>
#endif

#include "winnt_def.h"
#include "section.h"

namespace pe {

class exported_names_table {
public:
    using name_type = const char*;
    using ordinal_type = export_ordinal_t;

    class iterator;

    struct entry {
        entry();
        entry(const entry& entry) = default;
        entry& operator=(const entry& other) = default;

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] name_type name() const;
        [[nodiscard]] ordinal_type ordinal() const;
        [[nodiscard]] ordinal_type baised_ordinal() const;

    private:
        entry(const ImageExportDirectory* directory, name_type name, ordinal_type ordinal);

        const ImageExportDirectory* m_directory;
        name_type m_name;
        ordinal_type m_ordinal;

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

        iterator(const ImageExportDirectory* directory, const section& section,
                 const rva_t* name_pointer_ptr, const export_ordinal_t* ordinal_table_ptr);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        const ImageExportDirectory* m_directory;
        const section& m_section;

        const rva_t* m_name_pointer_ptr;
        const export_ordinal_t* m_ordinal_table_ptr;
    };

    exported_names_table(const ImageExportDirectory* directory, section section);

    [[nodiscard]] size_t count() const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

    [[nodiscard]] entry operator[](name_type name) const;
    [[nodiscard]] entry operator[](ordinal_type ordinal) const;

private:
    const ImageExportDirectory* m_directory;
    section m_section;
};

class export_table {
public:
    class iterator;

    struct entry {
        entry(const entry& other) = default;
        entry& operator=(const entry& other) = default;

        [[nodiscard]] rva_t rva() const;
        [[nodiscard]] export_ordinal_t ordinal() const;
        [[nodiscard]] export_ordinal_t baised_ordinal() const;

    private:
        entry(const ImageExportDirectory* directory, rva_t rva, export_ordinal_t ordinal);

        const ImageExportDirectory* m_directory;
        rva_t m_rva;
        export_ordinal_t m_ordinal;

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

        iterator(const ImageExportDirectory* directory, const section& section, const rva_t* name_pointer_ptr, export_ordinal_t current_ordinal);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        const ImageExportDirectory* m_directory;
        const section& m_section;

        const rva_t* m_address_table_ptr;
        export_ordinal_t m_current_ordinal;
    };

    export_table(const ImageExportDirectory* directory, section section);

    [[nodiscard]] bool is_valid() const;
    [[nodiscard]] const char* image_name() const;

    [[nodiscard]] export_ordinal_t to_unbaised(export_ordinal_t ordinal) const;
    [[nodiscard]] rva_t operator[](export_ordinal_t ordinal) const;

    [[nodiscard]] size_t count() const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

    [[nodiscard]] const exported_names_table& names() const;

private:
    const ImageExportDirectory* m_directory;
    section m_section;

    exported_names_table m_names_table;
};

}

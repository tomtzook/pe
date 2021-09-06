#pragma once

#include <iterator>

#include "winnt_def.h"
#include "section.h"

namespace pe {

class ExportedNames {
public:
    using name_type = const char*;
    using ordinal_type = export_ordinal_t;

    class iterator;

    struct entry {
    public:
        entry(const entry& entry);
        entry& operator=(const entry& other);

        name_type name() const;
        ordinal_type unbaised_ordinal() const;
        ordinal_type baised_ordinal() const;

    private:
        entry(const ImageExportDirectory* directory,
              name_type name, ordinal_type unbaised_ordinal);

        const ImageExportDirectory* m_directory;
        name_type m_name;
        ordinal_type m_unbaised_ordinal;

        friend iterator;
    };

    class iterator {
    public:
        using value_type = entry;
        using reference = entry;
        using pointer = entry;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(const ImageExportDirectory* directory, const Section& section,
                 const rva_t* namePointerPtr, const export_ordinal_t* ordinalTablePtr);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs);
        bool operator!=(const iterator& rhs);

    private:
        const ImageExportDirectory* m_directory;
        const Section& m_section;

        const rva_t* m_namePointerPtr;
        const export_ordinal_t* m_ordinalTablePtr;
    };

    ExportedNames(const ImageExportDirectory* directory, const Section& section);

    size_t count() const;

    iterator begin() const;
    iterator end() const;

    entry operator[](name_type name) const;

private:
    const ImageExportDirectory* m_directory;
    const Section& m_section;
};

class Export {
public:
    Export(const ImageExportDirectory* directory, const Section& section);

    export_ordinal_t to_unbaised(export_ordinal_t baised_ordinal) const;

    rva_t operator[](export_ordinal_t unbaised_ordinal) const;

    const ExportedNames& names() const;

    rva_t findExportByOrdinal(export_ordinal_t ordinal, bool is_baised = true) const;
    rva_t findExportByName(const char* name) const;
    export_ordinal_t findUnbaisedOrdinalByName(const char* name) const;

private:
    size_t findExportNameIndex(const char* name) const;

    const ImageExportDirectory* m_directory;
    const Section& m_section;

    const ExportedNames m_names;
};

}

#pragma once

#include <iterator>

#include "winnt_def.h"
#include "section.h"

namespace pe {

class ExportedNamesTable {
public:
    using name_type = const char*;
    using ordinal_type = export_ordinal_t;

    class iterator;

    struct entry {
    public:
        entry(const entry& entry);
        entry& operator=(const entry& other);

        name_type name() const;
        ordinal_type ordinal() const;
        ordinal_type baisedOrdinal() const;

    private:
        entry(const ImageExportDirectory* directory,
              name_type name, ordinal_type ordinal);

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

    ExportedNamesTable(const ImageExportDirectory* directory, Section section);

    size_t count() const;

    iterator begin() const;
    iterator end() const;

    entry operator[](name_type name) const;

private:
    const ImageExportDirectory* m_directory;
    Section m_section;
};

class ExportTable {
public:
    ExportTable(const ImageExportDirectory* directory, Section section);

    const char* imageName() const;

    export_ordinal_t toUnbaised(export_ordinal_t baisedOrdinal) const;
    rva_t operator[](export_ordinal_t ordinal) const;

    const ExportedNamesTable& names() const;

private:
    const ImageExportDirectory* m_directory;
    Section m_section;

    ExportedNamesTable m_namesTable;
};

}

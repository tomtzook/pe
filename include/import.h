#pragma once

#include "winnt_def.h"
#include "pe_base.h"
#include "section.h"

namespace pe {

class ImportedModule {
public:
    class iterator;

    struct entry {
    public:
        entry(const entry& entry);
        entry& operator=(const entry& other);

        bool is_ordinal() const;
        uint16_t ordinal() const;

        bool is_name() const;
        const char* name() const;

    private:
        entry(const ImageThunkData64* thunkData, const Section& section);

        const ImageThunkData64* m_thunkData;
        const Section& m_section;

        friend iterator;
    };

    class iterator {
    public:
        using value_type = entry;
        using reference = entry;
        using pointer = entry;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(const ImageThunkData64* thunkData, const Section& section);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs);
        bool operator!=(const iterator& rhs);

    private:
        const ImageThunkData64* m_thunkData;
        const Section& m_section;
    };

    ImportedModule(const ImageImportDescriptor* descriptor, const Section& section);

    const char* name() const;

    iterator begin() const;
    iterator end() const;

private:
    size_t findLastDataIndex() const;

    const ImageImportDescriptor* m_descriptor;
    const Section& m_section;
};

class ImportTable {
public:
    class iterator {
    public:
        using value_type = ImportedModule;
        using reference = ImportedModule;
        using pointer = ImportedModule;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(const ImageImportDescriptor* importDescriptor, const Section& section);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs);
        bool operator!=(const iterator& rhs);

    private:
        const ImageImportDescriptor* m_importDescriptor;
        const Section& m_section;
    };

    ImportTable(const ImageImportDescriptor* importDescriptors, Section section);

    iterator begin() const;
    iterator end() const;

private:
    size_t findLastDataIndex() const;

    const ImageImportDescriptor* m_importDescriptors;
    Section m_section;
};

}

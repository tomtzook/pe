
#include <cstring>
#include "except.h"
#include "export.h"


namespace pe {

ExportedNames::entry::entry(const entry& other)
        : m_directory(other.m_directory)
        , m_name(other.m_name)
        , m_unbaised_ordinal(other.m_unbaised_ordinal)
{}

ExportedNames::entry& ExportedNames::entry::operator=(const entry& other) {
    m_directory = other.m_directory;
    m_name = other.m_name;
    m_unbaised_ordinal = other.m_unbaised_ordinal;

    return *this;
}

ExportedNames::name_type ExportedNames::entry::name() const {
    return m_name;
}
ExportedNames::ordinal_type ExportedNames::entry::unbaised_ordinal() const {
    return m_unbaised_ordinal;
}
ExportedNames::ordinal_type ExportedNames::entry::baised_ordinal() const {
    return m_unbaised_ordinal + m_directory->OrdinalBase;
}

ExportedNames::entry::entry(const ImageExportDirectory* directory, name_type name, ordinal_type unbaised_ordinal)
    : m_directory(directory)
    , m_name(name)
    , m_unbaised_ordinal(unbaised_ordinal)
{}

ExportedNames::iterator::iterator(const ImageExportDirectory* directory, const Section& section,
         const rva_t* namePointerPtr, const export_ordinal_t* ordinalTablePtr)
        : m_directory(directory)
        , m_section(section)
        , m_namePointerPtr(namePointerPtr)
        , m_ordinalTablePtr(ordinalTablePtr)
{}

ExportedNames::iterator& ExportedNames::iterator::operator++() {
    m_namePointerPtr++;
    m_ordinalTablePtr++;
    return *this;
}
ExportedNames::iterator& ExportedNames::iterator::operator--() {
    m_namePointerPtr--;
    m_ordinalTablePtr--;
    return *this;
}

ExportedNames::iterator::reference ExportedNames::iterator::operator*() {
    return {m_directory, m_section.rvaToPointer<char>(*m_namePointerPtr), *m_ordinalTablePtr};
}
ExportedNames::iterator::pointer ExportedNames::iterator::operator->() {
    return {m_directory, m_section.rvaToPointer<char>(*m_namePointerPtr), *m_ordinalTablePtr};
}

bool ExportedNames::iterator::operator==(const iterator& rhs) {
    return m_namePointerPtr == rhs.m_namePointerPtr;
}
bool ExportedNames::iterator::operator!=(const iterator& rhs) {
    return m_namePointerPtr != rhs.m_namePointerPtr;
}

ExportedNames::ExportedNames(const ImageExportDirectory* directory, const Section& section)
        : m_directory(directory)
        , m_section(section)
{}

size_t ExportedNames::count() const {
    return m_directory->NumberOfNames;
}

ExportedNames::iterator ExportedNames::begin() const {
    auto namePointerTable = m_section.rvaToPointer<rva_t>(m_directory->AddressOfNames);
    auto ordinalTable = m_section.rvaToPointer<export_ordinal_t>(m_directory->AddressOfNameOrdinals);

    return iterator(m_directory, m_section, namePointerTable, ordinalTable);
}
ExportedNames::iterator ExportedNames::end() const {
    auto namePointerTable = m_section.rvaToPointer<rva_t>(m_directory->AddressOfNames);
    auto ordinalTable = m_section.rvaToPointer<export_ordinal_t>(m_directory->AddressOfNameOrdinals);

    return iterator(m_directory, m_section,
                    namePointerTable + count(),
                    ordinalTable + count());
}

ExportedNames::entry ExportedNames::operator[](name_type name) const {
    for (const auto& entry : *this) {
        if(0 == strcmp(name, entry.name())) {
            return entry;
        }
    }

    throw NotFoundException(name);
}

Export::Export(const ImageExportDirectory* directory, const Section& section)
    : m_directory(directory)
    , m_section(section)
    , m_names(m_directory, m_section)
{}

export_ordinal_t Export::to_unbaised(export_ordinal_t baised_ordinal) const {
    return baised_ordinal - m_directory->OrdinalBase;
}

rva_t Export::operator[](export_ordinal_t unbaised_ordinal) const {
    auto addressTable = m_section.rvaToPointer<rva_t>(m_directory->AddressOfFunctions);
    rva_t rva = addressTable[unbaised_ordinal];

    // If the address specified is not within the export section
    // (as defined by the address and length that are indicated in the optional header),
    // the field is an export RVA, which is an actual address in code or data.
    // Otherwise, the field is a forwarder RVA, which names a symbol in another DLL.
    if (m_section.containsRva(rva)) {
        throw RvaIsForwarderException(unbaised_ordinal);
    }

    return rva;
}

const ExportedNames& Export::names() const {
    return m_names;
}

rva_t Export::findExportByOrdinal(export_ordinal_t ordinal, bool is_baised) const {
    if (is_baised) {
        ordinal -= m_directory->OrdinalBase;
    }

    auto addressTable = m_section.rvaToPointer<rva_t>(m_directory->AddressOfFunctions);
    rva_t rva = addressTable[ordinal];

    // If the address specified is not within the export section
    // (as defined by the address and length that are indicated in the optional header),
    // the field is an export RVA, which is an actual address in code or data.
    // Otherwise, the field is a forwarder RVA, which names a symbol in another DLL.
    if (m_section.containsRva(rva)) {
        throw RvaIsForwarderException(ordinal);
    }

    return rva;
}

rva_t Export::findExportByName(const char* name) const {
    export_ordinal_t ordinal = findUnbaisedOrdinalByName(name);
    return findExportByOrdinal(ordinal, false);
}

export_ordinal_t Export::findUnbaisedOrdinalByName(const char* name) const {
    size_t nameIndex = findExportNameIndex(name);

    auto ordinalTable = m_section.rvaToPointer<export_ordinal_t>(m_directory->AddressOfNameOrdinals);
    return ordinalTable[nameIndex];
}

size_t Export::findExportNameIndex(const char* name) const {
    auto namePointerTable = m_section.rvaToPointer<rva_t>(m_directory->AddressOfNames);
    for (size_t i = 0; i < m_directory->NumberOfNames; i++) {
        auto exportName = m_section.rvaToPointer<char>(namePointerTable[i]);
        if (0 == strcmp(name, exportName)) {
            return i;
        }
    }

    throw NotFoundException(name);
}

}

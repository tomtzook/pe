
#include <cstring>
#include "except.h"
#include "export.h"


namespace pe {

ExportedNamesTable::entry::entry(const entry& other)
        : m_directory(other.m_directory)
        , m_name(other.m_name)
        , m_ordinal(other.m_ordinal)
{}

ExportedNamesTable::entry& ExportedNamesTable::entry::operator=(const entry& other) {
    m_directory = other.m_directory;
    m_name = other.m_name;
    m_ordinal = other.m_ordinal;

    return *this;
}

ExportedNamesTable::name_type ExportedNamesTable::entry::name() const {
    return m_name;
}
ExportedNamesTable::ordinal_type ExportedNamesTable::entry::ordinal() const {
    return m_ordinal;
}
ExportedNamesTable::ordinal_type ExportedNamesTable::entry::baisedOrdinal() const {
    return m_ordinal + m_directory->OrdinalBase;
}

ExportedNamesTable::entry::entry(const ImageExportDirectory* directory, name_type name, ordinal_type ordinal)
    : m_directory(directory)
    , m_name(name)
    , m_ordinal(ordinal)
{}

ExportedNamesTable::iterator::iterator(const ImageExportDirectory* directory, const Section& section,
                                       const rva_t* namePointerPtr, const export_ordinal_t* ordinalTablePtr)
        : m_directory(directory)
        , m_section(section)
        , m_namePointerPtr(namePointerPtr)
        , m_ordinalTablePtr(ordinalTablePtr)
{}

ExportedNamesTable::iterator& ExportedNamesTable::iterator::operator++() {
    m_namePointerPtr++;
    m_ordinalTablePtr++;
    return *this;
}
ExportedNamesTable::iterator& ExportedNamesTable::iterator::operator--() {
    m_namePointerPtr--;
    m_ordinalTablePtr--;
    return *this;
}

ExportedNamesTable::iterator::reference ExportedNamesTable::iterator::operator*() {
    return {m_directory, m_section.rvaToPointer<char>(*m_namePointerPtr), *m_ordinalTablePtr};
}
ExportedNamesTable::iterator::pointer ExportedNamesTable::iterator::operator->() {
    return {m_directory, m_section.rvaToPointer<char>(*m_namePointerPtr), *m_ordinalTablePtr};
}

bool ExportedNamesTable::iterator::operator==(const iterator& rhs) {
    return m_namePointerPtr == rhs.m_namePointerPtr;
}
bool ExportedNamesTable::iterator::operator!=(const iterator& rhs) {
    return m_namePointerPtr != rhs.m_namePointerPtr;
}

ExportedNamesTable::ExportedNamesTable(const ImageExportDirectory* directory, Section section)
        : m_directory(directory)
        , m_section(section)
{}

size_t ExportedNamesTable::count() const {
    return m_directory->NumberOfNames;
}

ExportedNamesTable::iterator ExportedNamesTable::begin() const {
    auto namePointerTable = m_section.rvaToPointer<rva_t>(m_directory->AddressOfNames);
    auto ordinalTable = m_section.rvaToPointer<export_ordinal_t>(m_directory->AddressOfNameOrdinals);

    return iterator(m_directory, m_section, namePointerTable, ordinalTable);
}
ExportedNamesTable::iterator ExportedNamesTable::end() const {
    auto namePointerTable = m_section.rvaToPointer<rva_t>(m_directory->AddressOfNames);
    auto ordinalTable = m_section.rvaToPointer<export_ordinal_t>(m_directory->AddressOfNameOrdinals);

    return iterator(m_directory, m_section,
                    namePointerTable + count(),
                    ordinalTable + count());
}

ExportedNamesTable::entry ExportedNamesTable::operator[](name_type name) const {
    for (const auto& entry : *this) {
        if(0 == strcmp(name, entry.name())) {
            return entry;
        }
    }

    throw NotFoundException(name);
}

ExportTable::ExportTable(const ImageExportDirectory* directory, Section section)
    : m_directory(directory)
    , m_section(section)
    , m_namesTable(m_directory, m_section)
{}

const char* ExportTable::imageName() const {
    return m_section.rvaToPointer<char>(m_directory->Name);
}

export_ordinal_t ExportTable::toUnbaised(export_ordinal_t baisedOrdinal) const {
    return baisedOrdinal - m_directory->OrdinalBase;
}

rva_t ExportTable::operator[](export_ordinal_t ordinal) const {
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

const ExportedNamesTable& ExportTable::names() const {
    return m_namesTable;
}

}

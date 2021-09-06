
#include <cstring>
#include "except.h"
#include "export.h"


namespace pe {

Export::Export(const ImageExportDirectory* directory, const Section& section)
    : m_directory(directory)
    , m_section(section)
{}

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

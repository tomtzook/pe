#pragma once

#include "winnt_def.h"
#include "section.h"

namespace pe {

class Export {
public:
    Export(const ImageExportDirectory* directory, const Section& section);

    rva_t findExportByOrdinal(export_ordinal_t ordinal, bool is_baised = true) const;
    rva_t findExportByName(const char* name) const;
    export_ordinal_t findUnbaisedOrdinalByName(const char* name) const;

private:
    size_t findExportNameIndex(const char* name) const;

    const ImageExportDirectory* m_directory;
    const Section& m_section;
};

}

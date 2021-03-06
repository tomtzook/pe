#pragma once

#include <vector>
#include <optional>

#include "winnt_def.h"
#include "pe_base.h"
#include "section.h"
#include "export.h"
#include "import.h"

namespace pe {

class Image {
public:
    explicit Image(const void* peBuffer);

    const PeHeaders& headers() const;

    const ImageSections& sections() const;
    size_t rvaToOffset(rva_t rva) const;
    template<typename T>
    const T* rvaToPointer(rva_t rva) const {
        size_t offset = rvaToOffset(rva);
        return reinterpret_cast<const T*>(m_headers.base() + offset);
    }

    bool hasExportTable() const;
    const ExportTable& exportTable() const;

    bool hasImportTable() const;
    const ImportTable& importTable() const;

private:
    void loadSections();

    PeHeaders m_headers;

    const ImageSections m_sections;
    std::optional<ExportTable> m_exportTable;
    std::optional<ImportTable> m_importTable;
};

}

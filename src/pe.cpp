
#include <cstring>
#include "except.h"
#include "pe.h"


namespace pe {

Image::Image(const void* peBuffer)
    : m_headers(peBuffer)
    , m_sections(m_headers) {
    loadSections();
}

const PeHeaders& Image::headers() const {
    return m_headers;
}

size_t Image::rvaToOffset(rva_t rva) const {
    for (const auto& section : sections()) {
        if (section.containsRva(rva)) {
            return section.rvaToOffset(rva);
        }
    }

    throw RvaNotInImageException(rva);
}

const ImageSections& Image::sections() const {
    return m_sections;
}

bool Image::hasExportTable() const {
    return m_exportTable.has_value();
}

const ExportTable& Image::exportTable() const {
    if (m_exportTable.has_value()) {
        return m_exportTable.value();
    }

    throw NoExportTableException();
}

bool Image::hasImportTable() const {
    return m_importTable.has_value();
}

const ImportTable& Image::importTable() const {
    if (m_importTable.has_value()) {
        return m_importTable.value();
    }

    throw NoImportTableException();
}

void Image::loadSections() {
    // load export
    try {
        auto exportDataDirectory = m_headers.dataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);
        auto section = m_sections[exportDataDirectory->VirtualAddress];
        auto exportDirectory = section.rvaToPointer<ImageExportDirectory>(exportDataDirectory->VirtualAddress);

        m_exportTable.emplace(ExportTable(exportDirectory, section));
    } catch (const DataDirectoryNotPresent&) {
        // no export section
    } catch (const RvaNotInImageException&) {
        // no export section
    }

    // load import
    try {
        auto importDataDirectory = m_headers.dataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
        auto section = m_sections[importDataDirectory->VirtualAddress];
        auto importDirectory = section.rvaToPointer<ImageImportDescriptor>(importDataDirectory->VirtualAddress);

        m_importTable.emplace(ImportTable(importDirectory, section));
    } catch (const DataDirectoryNotPresent&) {
        // no import section
    } catch (const RvaNotInImageException&) {
        // no export section
    }
}

}

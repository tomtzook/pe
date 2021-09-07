
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

    throw NameNotFoundException(".edata");
}

void Image::loadSections() {
    // load export
    try {
        auto section = m_sections[".edata"];
        auto exportDataDirectory = m_headers.dataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);
        auto exportDirectory = rvaToPointer<ImageExportDirectory>(exportDataDirectory->VirtualAddress);

        m_exportTable.emplace(ExportTable(exportDirectory, section));
    } catch (const NameNotFoundException& ex) {}
}

}

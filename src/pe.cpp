
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

bool Image::hasExportSection() const {
    return m_export.has_value();
}

const Export& Image::exportSection() const {
    if (m_export.has_value()) {
        return m_export.value();
    }

    throw NotFoundException(".edata");
}

void Image::loadSections() {
    // load export
    try {
        auto section = m_sections[".edata"];
        auto exportDataDirectory = m_headers.dataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);
        auto exportDirectory = rvaToPointer<ImageExportDirectory>(exportDataDirectory->VirtualAddress);

        m_export.emplace(Export(exportDirectory, section));
    } catch (const NotFoundException& ex) {}
}

}

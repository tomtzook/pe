
#include <cstring>
#include "except.h"
#include "pe.h"


namespace pe {

Image::Image(const void* peBuffer)
    : m_buffer(reinterpret_cast<const uint8_t*>(peBuffer))
    , m_dosHeader(reinterpret_cast<const ImageDosHeader*>(peBuffer))
    , m_ntHeaders(reinterpret_cast<const ImageNtHeaders64*>(m_buffer + m_dosHeader->e_lfanew)) {
    checkValidHeaders();
    loadSections();
}

Image::~Image() {

}

size_t Image::size() const {
    return m_ntHeaders->OptionalHeader.SizeOfImage;
}

MachineType Image::machineType() const {
    return static_cast<MachineType>(m_ntHeaders->FileHeader.Machine);
}

Subsystem Image::subsystem() const {
    return static_cast<Subsystem>(m_ntHeaders->OptionalHeader.Subsystem);
}

size_t Image::sectionsCount() const {
    return m_ntHeaders->FileHeader.NumberOfSections;
}

size_t Image::rvaToOffset(rva_t rva) const {
    for (auto& section : sections()) {
        if (section.containsRva(rva)) {
            return section.rvaToOffset(rva);
        }
    }

    throw RvaNotInImageException(rva);
}

const ImageDataDirectory* Image::dataDirectory(DataDirectoryType type) const {
    if (type >= m_ntHeaders->OptionalHeader.NumberOfRvaAndSizes) {
        throw DataDirectoryNotPresent(type);
    }

    return m_ntHeaders->OptionalHeader.DataDirectory + type;
}

const std::vector<Section>& Image::sections() const {
    return m_sections;
}

const Section& Image::sectionByName(const char* name) const {
    for (auto& section : sections()) {
        if (0 == strcmp(name, section.name())) {
            return section;
        }
    }

    throw NotFoundException(name);
}

const Export& Image::exportSection() const {
    if (m_export.has_value()) {
        return m_export.value();
    }

    throw NotFoundException(".edata");
}


void Image::checkValidHeaders() {
    if (IMAGE_DOS_SIGNATURE != m_dosHeader->e_magic) {
        throw BadHeaderException(BadHeaderException::ProblemReason::DOS_MAGIC);
    }
    if (IMAGE_NT_SIGNATURE != m_ntHeaders->Signature) {
        throw BadHeaderException(BadHeaderException::ProblemReason::NT_SIGNATURE);
    }
}

void Image::loadSections() {
    const ImageSectionHeader* sectionHeaders = getSectionHeaders();
    for (size_t i = 0; i < sectionsCount(); i++) {
        m_sections.emplace_back(m_buffer, m_ntHeaders, &sectionHeaders[i]);
    }

    // load export
    try {
        const Section& section = sectionByName(".edata");
        auto exportDataDirectory = dataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);
        auto exportDirectory = rvaToPointer<ImageExportDirectory>(exportDataDirectory->VirtualAddress);

        m_export.emplace(Export(exportDirectory, section));
    } catch (const NotFoundException& ex) {}
}

const ImageSectionHeader* Image::getSectionHeaders() const {
    return reinterpret_cast<const ImageSectionHeader*>(
            reinterpret_cast<const uint8_t*>(&m_ntHeaders->OptionalHeader)
            + m_ntHeaders->FileHeader.SizeOfOptionalHeader);
}

}

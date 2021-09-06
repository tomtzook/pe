
#include <cstring>
#include "except.h"
#include "pe.h"


namespace pe {

ImageSections::iterator::iterator(const uint8_t* imageBase, const ImageNtHeaders64* ntHeaders,
         const ImageSectionHeader* header)
    : m_imageBase(imageBase)
    , m_ntHeaders(ntHeaders)
    , m_sectionHeader(header)
{}

ImageSections::iterator& ImageSections::iterator::operator++() {
    m_sectionHeader++;
    return *this;
}
ImageSections::iterator& ImageSections::iterator::operator--() {
    m_sectionHeader--;
    return *this;
}

ImageSections::iterator::reference ImageSections::iterator::operator*() {
    return {m_imageBase, m_ntHeaders, m_sectionHeader};
}
ImageSections::iterator::pointer ImageSections::iterator::operator->() {
    return {m_imageBase, m_ntHeaders, m_sectionHeader};
}

bool ImageSections::iterator::operator==(const iterator& rhs) {
    return m_sectionHeader == rhs.m_sectionHeader;
}
bool ImageSections::iterator::operator!=(const iterator& rhs) {
    return m_sectionHeader != rhs.m_sectionHeader;
}

ImageSections::ImageSections(const uint8_t* imageBase, const ImageNtHeaders64* ntHeaders)
    : m_imageBase(imageBase)
    , m_ntHeaders(ntHeaders)
    , m_sectionHeaders(reinterpret_cast<const ImageSectionHeader*>(
                               reinterpret_cast<const uint8_t*>(&m_ntHeaders->OptionalHeader)
                               + m_ntHeaders->FileHeader.SizeOfOptionalHeader))
{}

size_t ImageSections::count() const {
    return m_ntHeaders->FileHeader.NumberOfSections;
}

Section ImageSections::operator[](const char* name) const {
    for(const auto& section : *this) {
        if (0 == strcmp(name, section.name())) {
            return section;
        }
    }

    throw NotFoundException(name);
}

ImageSections::iterator ImageSections::begin() const {
    return iterator(m_imageBase, m_ntHeaders, m_sectionHeaders);
}
ImageSections::iterator ImageSections::end() const {
    return iterator(m_imageBase, m_ntHeaders, m_sectionHeaders + count());
}

Image::Image(const void* peBuffer)
    : m_buffer(reinterpret_cast<const uint8_t*>(peBuffer))
    , m_dosHeader(reinterpret_cast<const ImageDosHeader*>(peBuffer))
    , m_ntHeaders(reinterpret_cast<const ImageNtHeaders64*>(m_buffer + m_dosHeader->e_lfanew))
    , m_sections(m_buffer, m_ntHeaders) {
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

Characteristics Image::characteristics() const {
    return Characteristics {.data = m_ntHeaders->FileHeader.Characteristics};
}

Subsystem Image::subsystem() const {
    return static_cast<Subsystem>(m_ntHeaders->OptionalHeader.Subsystem);
}

DllCharacteristics Image::dllCharacteristics() const {
    return DllCharacteristics {.data = m_ntHeaders->OptionalHeader.DllCharacteristics};
}

size_t Image::sectionsCount() const {
    return m_ntHeaders->FileHeader.NumberOfSections;
}

size_t Image::rvaToOffset(rva_t rva) const {
    for (const auto& section : sections()) {
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


void Image::checkValidHeaders() {
    if (IMAGE_DOS_SIGNATURE != m_dosHeader->e_magic) {
        throw BadHeaderException(BadHeaderException::ProblemReason::DOS_MAGIC);
    }
    if (IMAGE_NT_SIGNATURE != m_ntHeaders->Signature) {
        throw BadHeaderException(BadHeaderException::ProblemReason::NT_SIGNATURE);
    }
    if (IMAGE_NT_OPTIONAL_HDR64_MAGIC != m_ntHeaders->OptionalHeader.Magic) {
        throw BadHeaderException(BadHeaderException::ProblemReason::OPTIONAL_HEADER_MAGIC);
    }

    Characteristics ch = characteristics();
    if (ch.bits.LineNumsStripped || ch.bits.LocalSymsStripped || ch.bits.AggressiveWsTrim ||
        ch.bits.BytesReversedLo || ch.bits.BytesReversedHi) {
        throw BadHeaderException(BadHeaderException::ProblemReason::USE_OF_DEPRECATED_CHARACTERISTIC);
    }
}

void Image::loadSections() {
    // load export
    try {
        auto section = m_sections[".edata"];
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

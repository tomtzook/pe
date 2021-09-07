
#include "except.h"
#include "pe_base.h"


namespace pe {

PeHeaders::PeHeaders(const void* base)
    : m_base(reinterpret_cast<const uint8_t*>(base))
    , m_dosHeader(reinterpret_cast<const ImageDosHeader*>(base))
    , m_ntHeaders(reinterpret_cast<const ImageNtHeaders64*>(m_base + m_dosHeader->e_lfanew)) {
    checkValidHeaders();
}

const uint8_t* PeHeaders::base() const {
    return m_base;
}

size_t PeHeaders::size() const {
    return m_ntHeaders->OptionalHeader.SizeOfImage;
}

MachineType PeHeaders::machineType() const {
    return static_cast<MachineType>(m_ntHeaders->FileHeader.Machine);
}

Characteristics PeHeaders::characteristics() const {
    return Characteristics {.data=m_ntHeaders->FileHeader.Characteristics};
}

Subsystem PeHeaders::subsystem() const {
    return static_cast<Subsystem>(m_ntHeaders->OptionalHeader.Subsystem);
}

DllCharacteristics PeHeaders::dllCharacteristics() const {
    return DllCharacteristics {.data=m_ntHeaders->OptionalHeader.DllCharacteristics};
}

const ImageDataDirectory* PeHeaders::dataDirectory(DataDirectoryType type) const {
    if (type >= m_ntHeaders->OptionalHeader.NumberOfRvaAndSizes) {
        throw DataDirectoryNotPresent(type);
    }

    return m_ntHeaders->OptionalHeader.DataDirectory + type;
}

const ImageSectionHeader* PeHeaders::getSectionHeaders() const {
    return reinterpret_cast<const ImageSectionHeader*>(
            reinterpret_cast<const uint8_t*>(&m_ntHeaders->OptionalHeader)
            + m_ntHeaders->FileHeader.SizeOfOptionalHeader);
}

size_t PeHeaders::sectionsCount() const {
    return m_ntHeaders->FileHeader.NumberOfSections;
}

size_t PeHeaders::sectionAlignment() const {
    return m_ntHeaders->OptionalHeader.SectionAlignment;
}

bool PeHeaders::hasEntryPoint() const {
    return 0 != m_ntHeaders->OptionalHeader.AddressOfEntryPoint;
}

rva_t PeHeaders::entryPointAddress() const {
    if (!hasEntryPoint()) {
        throw NoEntryPointException();
    }

    return m_ntHeaders->OptionalHeader.AddressOfEntryPoint;
}

void PeHeaders::checkValidHeaders() {
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

}

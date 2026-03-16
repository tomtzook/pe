
#include "pe_base.h"


namespace pe {

headers::headers(const void* base)
    : m_base(static_cast<const uint8_t*>(base))
    , m_dos_header(static_cast<const ImageDosHeader*>(base))
    , m_nt_headers(reinterpret_cast<const ImageNtHeaders64*>(m_base + m_dos_header->e_lfanew))
{}

bool headers::is_valid() const {
    if (IMAGE_DOS_SIGNATURE != m_dos_header->e_magic) {
        return false;
    }
    if (IMAGE_NT_SIGNATURE != m_nt_headers->Signature) {
        return false;
    }
    if (IMAGE_NT_OPTIONAL_HDR64_MAGIC != m_nt_headers->OptionalHeader.Magic) {
        return false;
    }

    const auto ch = characteristics();
    if (ch.bits.LineNumsStripped || ch.bits.LocalSymsStripped || ch.bits.AggressiveWsTrim ||
        ch.bits.BytesReversedLo || ch.bits.BytesReversedHi) {
        return false;
    }

    return true;
}

const uint8_t* headers::base() const {
    return m_base;
}

size_t headers::size() const {
    return m_nt_headers->OptionalHeader.SizeOfImage;
}

MachineType headers::machineType() const {
    return static_cast<MachineType>(m_nt_headers->FileHeader.Machine);
}

Characteristics headers::characteristics() const {
    return Characteristics {.data=m_nt_headers->FileHeader.Characteristics};
}

Subsystem headers::subsystem() const {
    return static_cast<Subsystem>(m_nt_headers->OptionalHeader.Subsystem);
}

DllCharacteristics headers::dllCharacteristics() const {
    return DllCharacteristics {.data=m_nt_headers->OptionalHeader.DllCharacteristics};
}

const ImageDataDirectory* headers::data_directory(DataDirectoryType type) const {
    if (type >= m_nt_headers->OptionalHeader.NumberOfRvaAndSizes) {
        return nullptr;
    }
    if (0 == m_nt_headers->OptionalHeader.DataDirectory[type].VirtualAddress) {
        return nullptr;
    }

    return m_nt_headers->OptionalHeader.DataDirectory + type;
}

const ImageSectionHeader* headers::section_headers() const {
    return reinterpret_cast<const ImageSectionHeader*>(
            reinterpret_cast<const uint8_t*>(&m_nt_headers->OptionalHeader)
            + m_nt_headers->FileHeader.SizeOfOptionalHeader);
}

size_t headers::sections_count() const {
    return m_nt_headers->FileHeader.NumberOfSections;
}

size_t headers::section_alignment() const {
    return m_nt_headers->OptionalHeader.SectionAlignment;
}

bool headers::has_entry_point() const {
    return 0 != m_nt_headers->OptionalHeader.AddressOfEntryPoint;
}

rva_t headers::entry_point_address() const {
    if (!has_entry_point()) {
        return static_cast<rva_t>(-1);
    }

    return m_nt_headers->OptionalHeader.AddressOfEntryPoint;
}

}

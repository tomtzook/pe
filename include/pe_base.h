#pragma once

#include "winnt_def.h"

namespace pe {

struct headers {
    explicit headers(const void* base);

    [[nodiscard]] bool is_valid() const;
    [[nodiscard]] const uint8_t* base() const;
    [[nodiscard]] size_t size() const;

    [[nodiscard]] MachineType machineType() const;
    [[nodiscard]] Characteristics characteristics() const;
    [[nodiscard]] Subsystem subsystem() const;
    [[nodiscard]] DllCharacteristics dllCharacteristics() const;

    [[nodiscard]] const ImageDataDirectory* data_directory(DataDirectoryType type) const;

    [[nodiscard]] const ImageSectionHeader* section_headers() const;
    [[nodiscard]] size_t sections_count() const;
    [[nodiscard]] size_t section_alignment() const;

    [[nodiscard]] bool has_entry_point() const;
    [[nodiscard]] rva_t entry_point_address() const;

private:
    const uint8_t* m_base;
    const ImageDosHeader* m_dos_header;
    const ImageNtHeaders64* m_nt_headers;
};

}

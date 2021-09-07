#pragma once

#include <cstddef>
#include "winnt_def.h"

namespace pe {

struct PeHeaders {
public:
    explicit PeHeaders(const void* base);

    const uint8_t* base() const;

    size_t size() const;
    MachineType machineType() const;
    Characteristics characteristics() const;
    Subsystem subsystem() const;
    DllCharacteristics dllCharacteristics() const;

    const ImageDataDirectory* dataDirectory(DataDirectoryType type) const;

    const ImageSectionHeader* getSectionHeaders() const;
    size_t sectionsCount() const;
    size_t sectionAlignment() const;

    bool hasEntryPoint() const;
    rva_t entryPointAddress() const;

private:
    void checkValidHeaders();

    const uint8_t* m_base;
    const ImageDosHeader* m_dosHeader;
    const ImageNtHeaders64* m_ntHeaders;
};

}

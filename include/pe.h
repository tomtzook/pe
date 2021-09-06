#pragma once

#include <vector>
#include <optional>

#include "winnt_def.h"
#include "section.h"
#include "export.h"

namespace pe {

class Image {
public:
    explicit Image(const void* peBuffer);
    ~Image();

    size_t size() const;
    MachineType machineType() const;
    Subsystem subsystem() const;
    size_t sectionsCount() const;

    size_t rvaToOffset(rva_t rva) const;
    template<typename T>
    const T* rvaToPointer(rva_t rva) const {
        size_t offset = rvaToOffset(rva);
        return reinterpret_cast<const T*>(m_buffer + offset);
    }

    const ImageDataDirectory* dataDirectory(DataDirectoryType type) const;

    const std::vector<Section>& sections() const;
    const Section& sectionByName(const char* name) const;

    const Export& exportSection() const;

private:
    void checkValidHeaders();
    void loadSections();

    const ImageSectionHeader* getSectionHeaders() const;

    const uint8_t* m_buffer;
    const ImageDosHeader* m_dosHeader;
    const ImageNtHeaders64* m_ntHeaders;

    std::vector<Section> m_sections;
    std::optional<Export> m_export;
};

}

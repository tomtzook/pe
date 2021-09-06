#pragma once

#include <vector>
#include <optional>

#include "winnt_def.h"
#include "section.h"
#include "export.h"

namespace pe {

class ImageSections {
public:
    class iterator {
    public:
        using value_type = Section;
        using reference = Section;
        using pointer = Section;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(const uint8_t* imageBase, const ImageNtHeaders64* ntHeaders,
                 const ImageSectionHeader* header);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs);
        bool operator!=(const iterator& rhs);

    private:
        const uint8_t* m_imageBase;
        const ImageNtHeaders64* m_ntHeaders;
        const ImageSectionHeader* m_sectionHeader;
    };

    ImageSections(const uint8_t* imageBase, const ImageNtHeaders64* ntHeaders);

    size_t count() const;

    Section operator[](const char* name) const;

    iterator begin() const;
    iterator end() const;

private:
    const uint8_t* m_imageBase;
    const ImageNtHeaders64* m_ntHeaders;
    const ImageSectionHeader* m_sectionHeaders;
};

class Image {
public:
    explicit Image(const void* peBuffer);
    ~Image();

    size_t size() const;
    MachineType machineType() const;
    Characteristics characteristics() const;
    Subsystem subsystem() const;
    DllCharacteristics dllCharacteristics() const;
    size_t sectionsCount() const;

    size_t rvaToOffset(rva_t rva) const;
    template<typename T>
    const T* rvaToPointer(rva_t rva) const {
        size_t offset = rvaToOffset(rva);
        return reinterpret_cast<const T*>(m_buffer + offset);
    }

    const ImageDataDirectory* dataDirectory(DataDirectoryType type) const;

    const ImageSections& sections() const;

    bool hasExportSection() const;
    const Export& exportSection() const;

private:
    void checkValidHeaders();
    void loadSections();

    const ImageSectionHeader* getSectionHeaders() const;

    const uint8_t* m_buffer;
    const ImageDosHeader* m_dosHeader;
    const ImageNtHeaders64* m_ntHeaders;

    const ImageSections m_sections;
    std::optional<Export> m_export;
};

}

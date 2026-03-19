
#include "pe.h"


namespace pe {

image::image(const void* buffer, const memory_alignment alignment)
    : m_headers(buffer)
    , m_sections(m_headers, alignment)
    , m_alignment(alignment)
{}

const uint8_t* image::base() const {
    return m_headers.base();
}

const uint8_t* image::end() const {
    return m_headers.base() + m_headers.image_size();
}

size_t image::size() const {
    return m_headers.image_size();
}

const headers& image::headers() const {
    return m_headers;
}

const section_list& image::sections() const {
    return m_sections;
}

export_table image::load_export_table() const {
    const auto data_directory = m_headers.data_directory(image_directory_entry_export);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return {nullptr, {m_headers, nullptr, m_alignment}};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageExportDirectory>(data_directory->VirtualAddress);

    return {directory, section};
}

import_table image::load_import_table() const {
    const auto data_directory = m_headers.data_directory(image_directory_entry_import);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return {nullptr, {m_headers, nullptr, m_alignment}};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageImportDescriptor>(data_directory->VirtualAddress);

    return {directory, section};
}

functions_table image::load_exception_table() const {
    const auto data_directory = m_headers.data_directory(image_directory_entry_exception);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return {nullptr, {m_headers, nullptr, m_alignment}};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageRuntimeFunctionEntry>(data_directory->VirtualAddress);

    return {directory, section};
}

debug_table image::load_debug_table() const {
    const auto data_directory = m_headers.data_directory(image_directory_entry_debug);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return debug_table{nullptr};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageDebugDirectory>(data_directory->VirtualAddress);

    return debug_table{directory};
}

bool image::is_image_base(const void* ptr) {
    const struct headers headers(ptr);
    return headers.is_valid();
}

}

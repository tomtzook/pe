
#include "pe.h"


namespace pe {

image::image(const void* buffer)
    : m_headers(buffer)
    , m_sections(m_headers)
{}

const headers& image::headers() const {
    return m_headers;
}

const section_list& image::sections() const {
    return m_sections;
}

export_table image::load_export_table() const {
    const auto data_directory = m_headers.data_directory(IMAGE_DIRECTORY_ENTRY_EXPORT);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return {nullptr, {m_headers, nullptr}};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageExportDirectory>(data_directory->VirtualAddress);

    return {directory, section};
}

import_table image::load_import_table() const {
    const auto data_directory = m_headers.data_directory(IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return {nullptr, {m_headers, nullptr}};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageImportDescriptor>(data_directory->VirtualAddress);

    return {directory, section};
}

functions_table image::load_exception_table() const {
    const auto data_directory = m_headers.data_directory(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return {nullptr, {m_headers, nullptr}};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageRuntimeFunctionEntry>(data_directory->VirtualAddress);

    return {directory, section};
}

debug_table image::load_debug_table() const {
    const auto data_directory = m_headers.data_directory(IMAGE_DIRECTORY_ENTRY_DEBUG);
    if (data_directory == nullptr || !m_sections.contains_rva(data_directory->VirtualAddress)) {
        return debug_table{nullptr};
    }

    const auto section = m_sections[data_directory->VirtualAddress];
    const auto directory = section.rva_to_pointer<ImageDebugDirectory>(data_directory->VirtualAddress);

    return debug_table{directory};
}

}

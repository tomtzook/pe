
#include "import.h"

namespace pe {

bool imported_module::entry::is_ordinal() const {
    return m_thunk_data->u1.AddressOfData & IMAGE_ORDINAL_FLAG64;
}

uint16_t imported_module::entry::ordinal() const {
    return static_cast<uint16_t>(m_thunk_data->u1.AddressOfData & ~IMAGE_ORDINAL_FLAG64);
}

bool imported_module::entry::is_name() const {
    return !is_ordinal() && m_section.contains_rva(m_thunk_data->u1.AddressOfData);
}

const char* imported_module::entry::name() const {
    const auto import_by_name = m_section.rva_to_pointer<ImageImportByName>(m_thunk_data->u1.AddressOfData);
    return reinterpret_cast<const char*>(import_by_name->Name);
}

imported_module::entry::entry(const ImageThunkData64* thunk_data, const section& section)
    : m_thunk_data(thunk_data)
    , m_section(section)
{}

imported_module::iterator::iterator(const ImageThunkData64* thunk_data, const section& section)
    : m_thunk_data(thunk_data)
    , m_section(section)
{}

imported_module::iterator& imported_module::iterator::operator++() {
    m_thunk_data++;
    return *this;
}

imported_module::iterator& imported_module::iterator::operator--() {
    m_thunk_data--;
    return *this;
}

imported_module::iterator::reference imported_module::iterator::operator*() {
    return {m_thunk_data, m_section};
}

imported_module::iterator::pointer imported_module::iterator::operator->() {
    return {m_thunk_data, m_section};
}

bool imported_module::iterator::operator==(const iterator& rhs) const {
    return m_thunk_data == rhs.m_thunk_data;
}

bool imported_module::iterator::operator!=(const iterator& rhs) const {
    return m_thunk_data != rhs.m_thunk_data;
}

imported_module::imported_module(const ImageImportDescriptor* descriptor, const section& section)
    : m_descriptor(descriptor)
    , m_section(section)
{}

const char* imported_module::name() const {
    if (m_descriptor->Name) {
        return m_section.rva_to_pointer<char>(m_descriptor->Name);
    }

    return "";
}

imported_module::iterator imported_module::begin() const {
    auto thunk_data = m_section.rva_to_pointer<ImageThunkData64>(m_descriptor->FirstThunk);
    return {thunk_data, m_section};
}

imported_module::iterator imported_module::end() const {
    const auto thunk_data = m_section.rva_to_pointer<ImageThunkData64>(m_descriptor->FirstThunk);
    return {thunk_data + find_last_data_index(), m_section};
}

size_t imported_module::find_last_data_index() const {
    const auto thunkData = m_section.rva_to_pointer<ImageThunkData64>(m_descriptor->FirstThunk);
    size_t i = 0;
    for (; thunkData[i].u1.AddressOfData; i++){}

    return i;
}

import_table::iterator::iterator(const ImageImportDescriptor* import_descriptor, const section& section)
    : m_import_descriptor(import_descriptor)
    , m_section(section)
{}

import_table::iterator& import_table::iterator::operator++() {
    m_import_descriptor++;
    return *this;
}

import_table::iterator& import_table::iterator::operator--() {
    m_import_descriptor--;
    return *this;
}

import_table::iterator::reference import_table::iterator::operator*() {
    return {m_import_descriptor, m_section};
}

import_table::iterator::pointer import_table::iterator::operator->() {
    return {m_import_descriptor, m_section};
}

bool import_table::iterator::operator==(const iterator& rhs) const {
    return m_import_descriptor == rhs.m_import_descriptor;
}

bool import_table::iterator::operator!=(const iterator& rhs) const {
    return m_import_descriptor != rhs.m_import_descriptor;
}

import_table::import_table(const ImageImportDescriptor* import_descriptors, const section section)
    : m_import_descriptors(import_descriptors)
    , m_section(section)
{}

bool import_table::is_valid() const {
    return m_import_descriptors != nullptr;
}

import_table::iterator import_table::begin() const {
    return {m_import_descriptors, m_section};
}

import_table::iterator import_table::end() const {
    return {m_import_descriptors + find_last_data_index(), m_section};
}

size_t import_table::find_last_data_index() const {
    size_t i = 0;
    for (; m_import_descriptors[i].FirstThunk; i++){}

    return i;
}

}

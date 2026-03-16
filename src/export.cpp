
#include "export.h"


namespace pe {

bool exported_names_table::entry::is_valid() const {
    return m_directory != nullptr;
}

exported_names_table::name_type exported_names_table::entry::name() const {
    return m_name;
}

exported_names_table::ordinal_type exported_names_table::entry::ordinal() const {
    return m_ordinal;
}

exported_names_table::ordinal_type exported_names_table::entry::baised_ordinal() const {
    return m_ordinal + m_directory->OrdinalBase;
}

exported_names_table::entry::entry(const ImageExportDirectory* directory, const name_type name, const ordinal_type ordinal)
    : m_directory(directory)
    , m_name(name)
    , m_ordinal(ordinal)
{}

exported_names_table::entry::entry()
    : m_directory(nullptr)
    , m_name()
    , m_ordinal()
{}

exported_names_table::iterator::iterator(const ImageExportDirectory* directory, const section& section,
    const rva_t* name_pointer_ptr, const export_ordinal_t* ordinal_table_ptr)
        : m_directory(directory)
        , m_section(section)
        , m_name_pointer_ptr(name_pointer_ptr)
        , m_ordinal_table_ptr(ordinal_table_ptr)
{}

exported_names_table::iterator& exported_names_table::iterator::operator++() {
    m_name_pointer_ptr++;
    m_ordinal_table_ptr++;
    return *this;
}

exported_names_table::iterator& exported_names_table::iterator::operator--() {
    m_name_pointer_ptr--;
    m_ordinal_table_ptr--;
    return *this;
}

exported_names_table::iterator::reference exported_names_table::iterator::operator*() {
    return {m_directory, m_section.rva_to_pointer<char>(*m_name_pointer_ptr), *m_ordinal_table_ptr};
}

exported_names_table::iterator::pointer exported_names_table::iterator::operator->() {
    return {m_directory, m_section.rva_to_pointer<char>(*m_name_pointer_ptr), *m_ordinal_table_ptr};
}

bool exported_names_table::iterator::operator==(const iterator& rhs) const {
    return m_name_pointer_ptr == rhs.m_name_pointer_ptr;
}

bool exported_names_table::iterator::operator!=(const iterator& rhs) const {
    return m_name_pointer_ptr != rhs.m_name_pointer_ptr;
}

exported_names_table::exported_names_table(const ImageExportDirectory* directory, const section section)
        : m_directory(directory)
        , m_section(section)
{}

size_t exported_names_table::count() const {
    return m_directory->NumberOfNames;
}

exported_names_table::iterator exported_names_table::begin() const {
    const auto name_pointer_table = m_section.rva_to_pointer<rva_t>(m_directory->AddressOfNames);
    const auto ordinal_table = m_section.rva_to_pointer<export_ordinal_t>(m_directory->AddressOfNameOrdinals);

    return {m_directory, m_section, name_pointer_table, ordinal_table};
}

exported_names_table::iterator exported_names_table::end() const {
    const auto name_pointer_table = m_section.rva_to_pointer<rva_t>(m_directory->AddressOfNames);
    const auto ordinal_table = m_section.rva_to_pointer<export_ordinal_t>(m_directory->AddressOfNameOrdinals);

    return {m_directory, m_section, name_pointer_table + count(), ordinal_table + count()};
}

exported_names_table::entry exported_names_table::operator[](const name_type name) const {
    for (const auto& entry : *this) {
        if(0 == __builtin_strcmp(name, entry.name())) {
            return entry;
        }
    }

    return {};
}

exported_names_table::entry exported_names_table::operator[](const ordinal_type ordinal) const {
    for (const auto& entry : *this) {
        if(entry.ordinal() == ordinal) {
            return entry;
        }
    }

    return {};
}

rva_t export_table::entry::rva() const {
    return m_rva;
}

export_ordinal_t export_table::entry::ordinal() const {
    return m_ordinal;
}

export_ordinal_t export_table::entry::baised_ordinal() const {
    return m_ordinal + m_directory->OrdinalBase;
}

export_table::entry::entry(const ImageExportDirectory* directory, const rva_t rva, const export_ordinal_t ordinal)
      : m_directory(directory)
      , m_rva(rva)
      , m_ordinal(ordinal)
{}

export_table::iterator::iterator(const ImageExportDirectory* directory, const section& section,
         const rva_t* name_pointer_ptr, const export_ordinal_t current_ordinal)
     : m_directory(directory)
     , m_section(section)
     , m_address_table_ptr(name_pointer_ptr)
     , m_current_ordinal(current_ordinal)
{}

export_table::iterator& export_table::iterator::operator++() {
    m_address_table_ptr++;
    m_current_ordinal++;
    return *this;
}

export_table::iterator& export_table::iterator::operator--() {
    m_address_table_ptr--;
    m_current_ordinal--;
    return *this;
}

export_table::iterator::reference export_table::iterator::operator*() {
    return {m_directory, *m_address_table_ptr, m_current_ordinal};
}

export_table::iterator::pointer export_table::iterator::operator->() {
    return {m_directory, *m_address_table_ptr, m_current_ordinal};
}

bool export_table::iterator::operator==(const iterator& rhs) const {
    return m_address_table_ptr == rhs.m_address_table_ptr;
}

bool export_table::iterator::operator!=(const iterator& rhs) const {
    return m_address_table_ptr != rhs.m_address_table_ptr;
}

export_table::export_table(const ImageExportDirectory* directory, const section section)
    : m_directory(directory)
    , m_section(section)
    , m_names_table(m_directory, m_section)
{}

bool export_table::is_valid() const {
    return m_directory != nullptr;
}

const char* export_table::image_name() const {
    return m_section.rva_to_pointer<char>(m_directory->Name);
}

export_ordinal_t export_table::to_unbaised(const export_ordinal_t ordinal) const {
    return ordinal - m_directory->OrdinalBase;
}

rva_t export_table::operator[](const export_ordinal_t ordinal) const {
    const auto addressTable = m_section.rva_to_pointer<rva_t>(m_directory->AddressOfFunctions);
    const auto rva = addressTable[ordinal];

    // If the address specified is not within the export section
    // (as defined by the address and length that are indicated in the optional header),
    // the field is an export RVA, which is an actual address in code or data.
    // Otherwise, the field is a forwarder RVA, which names a symbol in another DLL.
    if (m_section.contains_rva(rva)) {
        return static_cast<rva_t>(-1);
    }

    return rva;
}

size_t export_table::count() const {
    return m_directory->NumberOfFunctions;
}

export_table::iterator export_table::begin() const {
    const auto addressTable = m_section.rva_to_pointer<rva_t>(m_directory->AddressOfFunctions);
    return {m_directory, m_section, addressTable, 0};
}

export_table::iterator export_table::end() const {
    auto address_table = m_section.rva_to_pointer<rva_t>(m_directory->AddressOfFunctions);
    return {m_directory, m_section, address_table + count(), static_cast<export_ordinal_t>(count())};
}

const exported_names_table& export_table::names() const {
    return m_names_table;
}

}

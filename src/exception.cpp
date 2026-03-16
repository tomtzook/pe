
#include "exception.h"

namespace pe {

unwind_info::unwind_info(const UnwindInfo* info)
    : m_info(info)
{}
    
bool unwind_info::has_flag(const uint16_t flag) const {
    return (m_info->Flags & flag) != 0;
}

uint16_t unwind_info::flags() const {
    return m_info->Flags;
}

size_t unwind_info::prolog_size() const {
    return m_info->SizeOfProlog;
}

uint32_t unwind_info::frame_register() const {
    return m_info->FrameRegister;
}

uint32_t unwind_info::frame_register_offset() const {
    return m_info->FrameOffset;
}

size_t unwind_info::codes_count() const {
    return m_info->CountOfCodes;
}

const UnwindCode* unwind_info::code(const size_t index) const {
    const auto* start = reinterpret_cast<const uint8_t*>(m_info) + sizeof(UnwindInfo);
    return &reinterpret_cast<const UnwindCode*>(start)[index];
}

bool unwind_info::has_next() const {
    return has_flag(UNW_FLAG_CHAININFO);
}

unwind_info unwind_info::next() const {
    const auto* after = reinterpret_cast<const uint8_t*>(m_info) + sizeof(UnwindInfo) + (sizeof(UnwindCode) * m_info->CountOfCodes);
    return unwind_info{reinterpret_cast<const UnwindInfo*>(after)};
}

function_entry::function_entry(const ImageRuntimeFunctionEntry* entry)
    : m_entry(entry)
{}

rva_t function_entry::start() const {
    return m_entry->BeginAddress;
}

rva_t function_entry::end() const {
    return m_entry->EndAddress;
}

rva_t function_entry::unwind_info_rva() const {
    return m_entry->DUMMYUNIONNAME.UnwindInfoAddress;
}

unwind_info function_entry::find_unwind_info(const section_list& sections) const {
    const auto rva = unwind_info_rva();
    const auto ptr = sections.rva_to_pointer<UnwindInfo>(rva);
    return unwind_info(ptr);
}

functions_table::iterator::iterator(const ImageRuntimeFunctionEntry* function_entry, const section& section)
    : m_function_entry(function_entry)
    , m_section(section)
{}

functions_table::iterator& functions_table::iterator::operator++() {
    m_function_entry++;
    return *this;
}

functions_table::iterator& functions_table::iterator::operator--() {
    m_function_entry--;
    return *this;
}

functions_table::iterator::reference functions_table::iterator::operator*() {
    return function_entry{m_function_entry};
}

functions_table::iterator::pointer functions_table::iterator::operator->() {
    return function_entry{m_function_entry};
}

bool functions_table::iterator::operator==(const iterator& rhs) const {
    return m_function_entry == rhs.m_function_entry;
}

bool functions_table::iterator::operator!=(const iterator& rhs) const {
    return m_function_entry != rhs.m_function_entry;
}

functions_table::functions_table(const ImageRuntimeFunctionEntry* function_entries, const section section)
    : m_function_entries(function_entries)
    , m_section(section)
{}

bool functions_table::is_valid() const {
    return m_function_entries != nullptr;
}

functions_table::iterator functions_table::begin() const {
    return {m_function_entries, m_section};
}

functions_table::iterator functions_table::end() const {
    return {m_function_entries + find_last_data_index(), m_section};
}

size_t functions_table::find_last_data_index() const {
    size_t i = 0;
    for (; m_function_entries[i].BeginAddress; i++){}

    return i;
}

}

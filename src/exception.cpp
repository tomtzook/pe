
#include "exception.h"

namespace pe {

static size_t sizeof_code_entry(const UnwindCode* code) {
    const auto opcode = static_cast<UnwindCodeOpCode>(code->u.OpCode);
    switch (opcode) {
        case uwop_alloc_large:
            return code->u.OpInfo == 0 ? 2 : 3;
        case uwop_alloc_small:
        case uwop_push_nonvol:
        case uwop_set_fpreg:
        case uwop_save_nonvol:
        case uwop_save_nonvol_far:
        case uwop_epilog:
        case uwop_spare_code:
        case uwop_save_xmm128:
        case uwop_save_xmm128_far:
        case uwop_push_machframe:
            return 1;
        default:
            return 0;
    }
}

unwind_code::unwind_code(const UnwindCode* code)
    : m_code(code)
{}

UnwindCodeOpCode unwind_code::code() const {
    return static_cast<UnwindCodeOpCode>(m_code->u.OpCode);
}

uint16_t unwind_code::info_raw() const {
    return m_code->u.OpInfo;
}

bool unwind_code::info_contains_register() const {
    switch (code()) {
        case uwop_push_nonvol:
        case uwop_save_nonvol:
        case uwop_save_nonvol_far:
        case uwop_save_xmm128:
        case uwop_save_xmm128_far:
            return true;
        default:
            return false;
    }
}

UnwindCodeOpInfo unwind_code::info_register() const {
    return static_cast<UnwindCodeOpInfo>(m_code->u.OpInfo);
}

size_t unwind_code::offset() const {
    return m_code->u.Offset;
}

size_t unwind_code::allocation_size() const {
    switch (code()) {
        case uwop_alloc_large:
            if (m_code->u.OpInfo == 0) {
                return m_code[1].FrameOffset * 8;
            }
            return *reinterpret_cast<const unsigned int*>(m_code + 1);
        case uwop_alloc_small:
            return (m_code->u.OpInfo * 8) + 8;
        default:
            return 0;
    }
}

unwind_codes::iterator::iterator(const UnwindCode* code)
    : m_code(code)
{}

unwind_codes::iterator& unwind_codes::iterator::operator++() {
    m_code += sizeof_code_entry(m_code);
    return *this;
}

unwind_codes::iterator& unwind_codes::iterator::operator--() {
    m_code -= sizeof_code_entry(m_code);
    return *this;
}

unwind_codes::iterator::reference unwind_codes::iterator::operator*() {
    return unwind_code{m_code};
}

unwind_codes::iterator::pointer unwind_codes::iterator::operator->() {
    return unwind_code{m_code};
}

bool unwind_codes::iterator::operator==(const iterator& rhs) const {
    return m_code == rhs.m_code;
}

bool unwind_codes::iterator::operator!=(const iterator& rhs) const {
    return m_code != rhs.m_code;
}

unwind_codes::unwind_codes(const UnwindCode* codes, const size_t count)
    : m_codes(codes)
    , m_count(count)
{}

size_t unwind_codes::count() const {
    return m_count;
}

unwind_codes::iterator unwind_codes::begin() const {
    return iterator{m_codes};
}

unwind_codes::iterator unwind_codes::end() const {
    return iterator{m_codes + count()};
}

unwind_info::unwind_info(const UnwindInfo* info)
    : m_info(info)
{}

bool unwind_info::is_valid() const {
    return m_info != nullptr;
}

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

unwind_codes unwind_info::codes() const {
    const auto* start = reinterpret_cast<const UnwindCode*>(reinterpret_cast<const uint8_t*>(m_info) + sizeof(UnwindInfo));
    return unwind_codes{start, m_info->CountOfCodes};
}

bool unwind_info::has_next() const {
    return has_flag(unw_flag_chaininfo);
}

unwind_info unwind_info::next() const {
    if (!has_next()) {
        return unwind_info(nullptr);
    }

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

functions_table::iterator::iterator(const ImageRuntimeFunctionEntry* function_entry)
    : m_function_entry(function_entry)
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

functions_table::functions_table(const ImageRuntimeFunctionEntry* function_entries)
    : m_function_entries(function_entries)
{}

bool functions_table::is_valid() const {
    return m_function_entries != nullptr;
}

functions_table::iterator functions_table::begin() const {
    return iterator{m_function_entries};
}

functions_table::iterator functions_table::end() const {
    return iterator{m_function_entries + find_last_data_index()};
}

size_t functions_table::find_last_data_index() const {
    size_t i = 0;
    for (; m_function_entries[i].BeginAddress; i++){}

    return i;
}

}

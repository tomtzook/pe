#pragma once

#ifndef PE_NO_STD
#include <iterator>
#endif

#include "winnt_def.h"
#include "section.h"

namespace pe {

class unwind_code {
public:
    explicit unwind_code(const UnwindCode* code);

    [[nodiscard]] UnwindCodeOpCode code() const;
    [[nodiscard]] uint16_t info_raw() const;
    [[nodiscard]] bool info_contains_register() const;
    [[nodiscard]] UnwindCodeOpInfo info_register() const;
    [[nodiscard]] size_t offset() const;
    [[nodiscard]] size_t allocation_size() const;

private:
    const UnwindCode* m_code;
};

class unwind_codes {
public:
    class iterator {
    public:
        using value_type = unwind_code;
        using reference = unwind_code;
        using pointer = unwind_code;

#ifndef PE_NO_STD
        using iterator_category = std::bidirectional_iterator_tag;
#endif

        explicit iterator(const UnwindCode* code);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        const UnwindCode* m_code;
    };

    unwind_codes(const UnwindCode* codes, size_t count);

    [[nodiscard]] size_t count() const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

private:
    const UnwindCode* m_codes;
    const size_t m_count;
};

class unwind_info {
public:
    explicit unwind_info(const UnwindInfo* info);

    [[nodiscard]] bool is_valid() const;
    [[nodiscard]] bool has_flag(uint16_t flag) const;
    [[nodiscard]] uint16_t flags() const;
    [[nodiscard]] size_t prolog_size() const;
    [[nodiscard]] uint32_t frame_register() const;
    [[nodiscard]] uint32_t frame_register_offset() const;

    [[nodiscard]] size_t codes_count() const;
    [[nodiscard]] const UnwindCode* code(size_t index) const;
    [[nodiscard]] unwind_codes codes() const;

    [[nodiscard]] bool has_next() const;
    [[nodiscard]] unwind_info next() const;

private:
    const UnwindInfo* m_info;
};

class function_entry {
public:
    explicit function_entry(const ImageRuntimeFunctionEntry* entry);

    [[nodiscard]] rva_t start() const;
    [[nodiscard]] rva_t end() const;
    [[nodiscard]] rva_t unwind_info_rva() const;
    [[nodiscard]] unwind_info find_unwind_info(const section_list& sections) const;

private:
    const ImageRuntimeFunctionEntry* m_entry;
};

class functions_table {
public:
    class iterator {
    public:
        using value_type = function_entry;
        using reference = function_entry;
        using pointer = function_entry;

#ifndef PE_NO_STD
        using iterator_category = std::bidirectional_iterator_tag;
#endif

        explicit iterator(const ImageRuntimeFunctionEntry* function_entry);

        iterator& operator++();
        iterator& operator--();

        reference operator*();
        pointer operator->();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

    private:
        const ImageRuntimeFunctionEntry* m_function_entry;
    };

    explicit functions_table(const ImageRuntimeFunctionEntry* function_entries);

    [[nodiscard]] bool is_valid() const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

private:
    [[nodiscard]] size_t find_last_data_index() const;

    const ImageRuntimeFunctionEntry* m_function_entries;
};

}

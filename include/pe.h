#pragma once

#include "pe_base.h"
#include "section.h"
#include "export.h"
#include "exception.h"
#include "import.h"
#include "debug.h"

namespace pe {

class image {
public:
    image(const void* buffer, memory_alignment alignment);

    [[nodiscard]] const headers& headers() const;
    [[nodiscard]] const section_list& sections() const;

    [[nodiscard]] export_table load_export_table() const;
    [[nodiscard]] import_table load_import_table() const;
    [[nodiscard]] functions_table load_exception_table() const;
    [[nodiscard]] debug_table load_debug_table() const;

private:
    pe::headers m_headers;
    const section_list m_sections;
    const memory_alignment m_alignment;
};

}

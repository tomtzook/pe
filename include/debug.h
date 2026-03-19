#pragma once

#include "winnt_def.h"
#include "section.h"

namespace pe {

class debug_table {
public:
    explicit debug_table(const ImageDebugDirectory* directory);

    [[nodiscard]] bool is_valid() const;
    [[nodiscard]] DebugType type() const;

    template<typename T>
    [[nodiscard]] const T* data(const section_list& sections) const {
        return sections.rva_to_pointer<T>(m_directory->AddressOfRawData);
    }

private:
    const ImageDebugDirectory* m_directory;
};

}

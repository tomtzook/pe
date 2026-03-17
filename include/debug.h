#pragma once

#include "winnt_def.h"

namespace pe {

class debug_table {
public:
    explicit debug_table(const ImageDebugDirectory* directory);

private:
    const ImageDebugDirectory* m_directory;
};

}

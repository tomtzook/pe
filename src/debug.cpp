
#include "debug.h"

namespace pe {

debug_table::debug_table(const ImageDebugDirectory* directory)
    : m_directory(directory)
{}

bool debug_table::is_valid() const {
    return m_directory != nullptr;
}

DebugType debug_table::type() const {
    return static_cast<DebugType>(m_directory->Type);
}

}

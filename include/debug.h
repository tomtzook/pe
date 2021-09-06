#pragma once

#include <ostream>

#include "winnt_def.h"
#include "pe.h"
#include "export.h"


std::ostream& operator<<(std::ostream& os, const pe::PeHeaders& headers);
std::ostream& operator<<(std::ostream& os, const pe::Image& image);
std::ostream& operator<<(std::ostream& os, const pe::MachineType& type);
std::ostream& operator<<(std::ostream& os, const pe::Characteristics& ch);
std::ostream& operator<<(std::ostream& os, const pe::Subsystem& subsystem);
std::ostream& operator<<(std::ostream& os, const pe::DllCharacteristics& ch);

std::ostream& operator<<(std::ostream& os, const pe::Section& section);

std::ostream& operator<<(std::ostream& os, const pe::ExportedNamesTable::entry& entry);

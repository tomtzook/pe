
#include "debug.h"


#define FLAG_BIT_DEBUG(bit, str) \
    if (bit) {                              \
        if (has) {                          \
            os << " | ";                    \
        }                                   \
        os << str;                          \
        has = true;                         \
    }


std::ostream& operator<<(std::ostream& os, const pe::PeHeaders& headers) {
    os << "IMAGE size=0x" << std::hex << headers.size() <<
       ", machine=" << headers.machineType() <<
       ", subsystem=" << headers.subsystem() <<
       std::endl;
    os << "\tcharacteristics=" << headers.characteristics() << std::endl;
    os << "\tDLL characteristics=" << headers.dllCharacteristics();

    return os;
}

std::ostream& operator<<(std::ostream& os, const pe::Image& image) {
    os << image.headers();
    if (image.hasExportTable()) {
        os << std::endl << "\tname=" << image.exportTable().imageName();
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const pe::MachineType& type) {
    switch (type) {
        case pe::MachineType::MACHINE_UNKNOWN: os << "MACHINE_UNKNOWN"; break;
        case pe::MachineType::MACHINE_I860: os << "MACHINE_I860"; break;
        case pe::MachineType::MACHINE_I386: os << "MACHINE_I386"; break;
        case pe::MachineType::MACHINE_R3000: os << "MACHINE_R3000"; break;
        case pe::MachineType::MACHINE_R4000: os << "MACHINE_R4000"; break;
        case pe::MachineType::MACHINE_R10000: os << "MACHINE_R10000"; break;
        case pe::MachineType::MACHINE_WCEMIPSV2: os << "MACHINE_WCEMIPSV2"; break;
        case pe::MachineType::MACHINE_ALPHA: os << "MACHINE_ALPHA"; break;
        case pe::MachineType::MACHINE_SH3: os << "MACHINE_SH3"; break;
        case pe::MachineType::MACHINE_SH3DSP: os << "MACHINE_SH3DSP"; break;
        case pe::MachineType::MACHINE_SH3E: os << "MACHINE_SH3E"; break;
        case pe::MachineType::MACHINE_SH4: os << "MACHINE_SH4"; break;
        case pe::MachineType::MACHINE_SH5: os << "MACHINE_SH5"; break;
        case pe::MachineType::MACHINE_ARM: os << "MACHINE_ARM"; break;
        case pe::MachineType::MACHINE_THUMB: os << "MACHINE_THUMB"; break;
        case pe::MachineType::MACHINE_ARMNT: os << "MACHINE_ARMNT"; break;
        case pe::MachineType::MACHINE_AM33: os << "MACHINE_AM33"; break;
        case pe::MachineType::MACHINE_POWERPC: os << "MACHINE_POWERPC"; break;
        case pe::MachineType::MACHINE_POWERPCFP: os << "MACHINE_POWERPCFP"; break;
        case pe::MachineType::MACHINE_IA64: os << "MACHINE_IA64"; break;
        case pe::MachineType::MACHINE_MIPS16: os << "MACHINE_MIPS16"; break;
        case pe::MachineType::MACHINE_ALPHA64: os << "MACHINE_ALPHA64|AXP64"; break;
        case pe::MachineType::MACHINE_MIPSFPU: os << "MACHINE_MIPSFPU"; break;
        case pe::MachineType::MACHINE_MIPSFPU16: os << "MACHINE_MIPSFPU16"; break;
        case pe::MachineType::MACHINE_TRICORE: os << "MACHINE_TRICORE"; break;
        case pe::MachineType::MACHINE_CEF: os << "MACHINE_CEF"; break;
        case pe::MachineType::MACHINE_EBC: os << "MACHINE_EBC"; break;
        case pe::MachineType::MACHINE_AMD64: os << "MACHINE_AMD64"; break;
        case pe::MachineType::MACHINE_M32R: os << "MACHINE_M32R"; break;
        case pe::MachineType::MACHINE_CEE: os << "MACHINE_CEE"; break;
        case pe::MachineType::MACHINE_ARM64: os << "MACHINE_ARM64"; break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const pe::Characteristics& ch) {
    bool has = false;
    FLAG_BIT_DEBUG(ch.bits.RelocsStripped, "RELOC_STRIPPED");
    FLAG_BIT_DEBUG(ch.bits.ExecutableImage, "EXECUTABLE_IMAGE");
    FLAG_BIT_DEBUG(ch.bits.LargeAddressAware, "LARGE_ADDR_AWARE");
    FLAG_BIT_DEBUG(ch.bits.bit32Machine, "32BIT_MACHINE");
    FLAG_BIT_DEBUG(ch.bits.DebugStripped, "DEBUG_STRIPPED");
    FLAG_BIT_DEBUG(ch.bits.RemovableRunFromSwap, "REMOVABLE_RUN");
    FLAG_BIT_DEBUG(ch.bits.NetRunFromSwap, "NET_RUN");
    FLAG_BIT_DEBUG(ch.bits.FileSystem, "FILE_SYSTEM");
    FLAG_BIT_DEBUG(ch.bits.Dll, "DLL");
    FLAG_BIT_DEBUG(ch.bits.UpSystemOnly, "UP_SYSTEM");

    return os;
}

std::ostream& operator<<(std::ostream& os, const pe::Subsystem& subsystem) {
    switch (subsystem) {
        case pe::Subsystem::SUBSYSTEM_UNKNOWN: os << "SUBSYSTEM_UNKNOWN"; break;
        case pe::Subsystem::SUBSYSTEM_NATIVE: os << "SUBSYSTEM_NATIVE"; break;
        case pe::Subsystem::SUBSYSTEM_WINDOWS_GUI: os << "SUBSYSTEM_WINDOWS_GUI"; break;
        case pe::Subsystem::SUBSYSTEM_WINDOWS_CUI: os << "SUBSYSTEM_WINDOWS_CUI"; break;
        case pe::Subsystem::SUBSYSTEM_OS2_CUI: os << "SUBSYSTEM_OS2_CUI"; break;
        case pe::Subsystem::SUBSYSTEM_POSIX_CUI: os << "SUBSYSTEM_POSIX_CUI"; break;
        case pe::Subsystem::SUBSYSTEM_NATIVE_WINDOWS: os << "SUBSYSTEM_NATIVE_WINDOWS"; break;
        case pe::Subsystem::SUBSYSTEM_WINDOWS_CE_GUI: os << "SUBSYSTEM_WINDOWS_CE_GUI"; break;
        case pe::Subsystem::SUBSYSTEM_EFI_APPLICATION: os << "SUBSYSTEM_EFI_APPLICATION"; break;
        case pe::Subsystem::SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER: os << "SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER"; break;
        case pe::Subsystem::SUBSYSTEM_EFI_RUNTIME_DRIVER: os << "SUBSYSTEM_EFI_RUNTIME_DRIVER"; break;
        case pe::Subsystem::SUBSYSTEM_EFI_ROM: os << "SUBSYSTEM_EFI_ROM"; break;
        case pe::Subsystem::SUBSYSTEM_XBOX: os << "SUBSYSTEM_XBOX"; break;
        case pe::Subsystem::SUBSYSTEM_WINDOWS_BOOT_APPLICATION: os << "SUBSYSTEM_WINDOWS_BOOT_APPLICATION"; break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const pe::DllCharacteristics& ch) {
    bool has = false;
    FLAG_BIT_DEBUG(ch.bits.HighEntropyVa, "HIGH_ENTROPY_VA");
    FLAG_BIT_DEBUG(ch.bits.DynamicBase, "DYNAMIC_BASE");
    FLAG_BIT_DEBUG(ch.bits.ForceIntegrity, "BASE_INTEGRITY");
    FLAG_BIT_DEBUG(ch.bits.NxCompat, "NX_COMPAT");
    FLAG_BIT_DEBUG(ch.bits.NoIsolation, "NO_ISOLATION");
    FLAG_BIT_DEBUG(ch.bits.NoSeh, "NO_SEH");
    FLAG_BIT_DEBUG(ch.bits.NoBind, "NO_BIND");
    FLAG_BIT_DEBUG(ch.bits.AppContainer, "APP_CONTAINER");
    FLAG_BIT_DEBUG(ch.bits.WdmDriver, "WDM_DRIVER");
    FLAG_BIT_DEBUG(ch.bits.GuardCf, "GUARD_CF");
    FLAG_BIT_DEBUG(ch.bits.TerminalServerAware, "TERMINAL_SERVER_AWARE");

    return os;
}

std::ostream& operator<<(std::ostream& os, const pe::Section& section) {
    os << "SECTION name=" << section.name() << ", size=0x" << std::hex << section.alignedVirtualSize();
    return os;
}

std::ostream& operator<<(std::ostream& os, const pe::ExportedNamesTable::entry& entry) {
    os << "name=" << entry.name()
        << " unbaised=0x" << entry.ordinal()
        << " baised=0x" << entry.baisedOrdinal();

    return os;
}

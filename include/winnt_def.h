#pragma once

#ifndef PE_NO_STD
#include <cstdint>
#include <cstddef>
#endif


namespace pe {

#ifdef PE_NO_STD
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef int64_t intn_t;
typedef int64_t intptr_t;
typedef long int intmax_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint64_t uintn_t;
typedef uint64_t uintptr_t;
typedef unsigned long int uintmax_t;
#endif


constexpr uint16_t IMAGE_DOS_SIGNATURE = 0x5A4D; /* MZ */
constexpr uint32_t IMAGE_NT_SIGNATURE = 0x4550; /* PE */

constexpr uint32_t IMAGE_NT_OPTIONAL_HDR32_MAGIC = 0x10b;
constexpr uint32_t IMAGE_NT_OPTIONAL_HDR64_MAGIC = 0x20b;
constexpr uint32_t IMAGE_ROM_OPTIONAL_HDR_MAGIC = 0x107;


using rva_t = uint32_t;
using export_ordinal_t = uint16_t;

#pragma pack(push, 1)

struct ImageDosHeader {  // DOS .EXE header
    uint16_t e_magic;         // Magic number
    uint16_t e_cblp;          // uint8_ts on last page of file
    uint16_t e_cp;            // Pages in file
    uint16_t e_crlc;          // Relocations
    uint16_t e_cparhdr;       // Size of header in paragraphs
    uint16_t e_minalloc;      // Minimum extra paragraphs needed
    uint16_t e_maxalloc;      // Maximum extra paragraphs needed
    uint16_t e_ss;            // Initial (relative) SS value
    uint16_t e_sp;            // Initial SP value
    uint16_t e_csum;          // Checksum
    uint16_t e_ip;            // Initial IP value
    uint16_t e_cs;            // Initial (relative) CS value
    uint16_t e_lfarlc;        // File address of relocation table
    uint16_t e_ovno;          // Overlay number
    uint16_t e_res[4];        // Reserved uint16_ts
    uint16_t e_oemid;         // OEM identifier (for e_oeminfo)
    uint16_t e_oeminfo;       // OEM information; e_oemid specific
    uint16_t e_res2[10];      // Reserved uint16_ts
    uint32_t e_lfanew;        // File address of new exe header
};

enum MachineType {
    machine_unknown	= 0,
    machine_i860 = 0x014d,
    machine_i386 = 0x014c,
    machine_r3000 = 0x0162,
    machine_r4000 = 0x0166,
    machine_r10000 = 0x0168,
    machine_wcemipsv2 = 0x0169,
    machine_alpha = 0x0184,
    machine_sh3 = 0x01a2,
    machine_sh3dsp = 0x01a3,
    machine_sh3e = 0x01a4,
    machine_sh4 = 0x01a6,
    machine_sh5 = 0x01a8,
    machine_arm = 0x01c0,
    machine_thumb = 0x01c2,
    machine_armnt = 0x01c4,
    machine_am33 = 0x01d3,
    machine_powerpc	= 0x01f0,
    machine_powerpcfp = 0x01f1,
    machine_ia64 = 0x0200,
    machine_mips16 = 0x0266,
    machine_alpha64	= 0x0284,
    machine_mipsfpu	= 0x0366,
    machine_mipsfpu16 = 0x0466,
    machine_axp64 = machine_alpha64,
    machine_tricore	= 0x0520,
    machine_cef	= 0x0cef,
    machine_ebc	= 0x0ebc,
    machine_amd64 = 0x8664,
    machine_m32r = 0x9041,
    machine_cee	= 0xc0ee,
    machine_arm64 = 0x01c5
};

union Characteristics {
    uint16_t data;
    struct {
        uint16_t RelocsStripped : 1;
        uint16_t ExecutableImage : 1;
        uint16_t LineNumsStripped : 1;
        uint16_t LocalSymsStripped : 1;
        uint16_t AggressiveWsTrim : 1;
        uint16_t LargeAddressAware : 1;
        uint16_t reserved0 : 1;
        uint16_t BytesReversedLo : 1;
        uint16_t bit32Machine : 1;
        uint16_t DebugStripped : 1;
        uint16_t RemovableRunFromSwap : 1;
        uint16_t NetRunFromSwap : 1;
        uint16_t FileSystem : 1;
        uint16_t Dll : 1;
        uint16_t UpSystemOnly : 1;
        uint16_t BytesReversedHi : 1;
    } bits;
};
static_assert(sizeof(Characteristics) == sizeof(uint16_t), "sizeof(Characteristics)");

struct ImageFileHeader {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
};
static_assert(sizeof(ImageFileHeader) == 20, "sizeof(File Header)");

struct ImageDataDirectory {
    uint32_t VirtualAddress;
    uint32_t Size;
};

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

enum DataDirectoryType {
    image_directory_entry_export = 0,
    image_directory_entry_import = 1,
    image_directory_entry_resource = 2,
    image_directory_entry_exception = 3,
    image_directory_entry_security = 4,
    image_directory_entry_basereloc = 5,
    image_directory_entry_debug = 6,
    image_directory_entry_copyright = 7,
    image_directory_entry_globalptr = 8,   /* (MIPS GP) */
    image_directory_entry_tls = 9,
    image_directory_entry_load_config = 10,
    image_directory_entry_bound_import = 11,
    image_directory_entry_iat = 12,  /* Import Address Table */
    image_directory_entry_delay_import = 13,
    image_directory_entry_com_descriptor = 14,

    image_directory_max
};

enum Subsystem {
    subsystem_unknown = 0,
    subsystem_native = 1,
    subsystem_windows_gui = 2,	/* Windows GUI subsystem */
    subsystem_windows_cui = 3,	/* Windows character subsystem */
    subsystem_os2_cui = 5,
    subsystem_posix_cui = 7,
    subsystem_native_windows = 8,	/* native Win9x driver */
    subsystem_windows_ce_gui = 9,	/* Windows CE subsystem */
    subsystem_efi_application = 10,
    subsystem_efi_boot_service_driver = 11,
    subsystem_efi_runtime_driver = 12,
    subsystem_efi_rom = 13,
    subsystem_xbox = 14,
    subsystem_windows_boot_application = 16,
};

union DllCharacteristics {
    uint16_t data;
    struct {
        uint16_t reserved0 : 5;
        uint16_t HighEntropyVa : 1;
        uint16_t DynamicBase : 1;
        uint16_t ForceIntegrity : 1;
        uint16_t NxCompat : 1;
        uint16_t NoIsolation : 1;
        uint16_t NoSeh : 1;
        uint16_t NoBind : 1;
        uint16_t AppContainer : 1;
        uint16_t WdmDriver : 1;
        uint16_t GuardCf : 1;
        uint16_t TerminalServerAware : 1;
    } bits;
};
static_assert(sizeof(DllCharacteristics) == sizeof(uint16_t), "sizeof(DLL Characteristics)");

struct ImageOptionalHeaders64 {
    uint16_t Magic; /* 0x20b */
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint64_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint64_t SizeOfStackReserve;
    uint64_t SizeOfStackCommit;
    uint64_t SizeOfHeapReserve;
    uint64_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    ImageDataDirectory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};
static_assert(sizeof(ImageOptionalHeaders64) == 240, "sizeof(Optional Header 64)");

struct ImageNtHeaders64 {
    uint32_t Signature;
    ImageFileHeader FileHeader;
    ImageOptionalHeaders64 OptionalHeader;
};

struct ImageOptionalHeaders32 {
    /* Standard fields */
    uint16_t Magic; /* 0x10b or 0x107 */	/* 0x00 */
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;		/* 0x10 */
    uint32_t BaseOfCode;
    uint32_t BaseOfData;
    /* NT additional fields */
    uint32_t ImageBase;
    uint32_t SectionAlignment;		/* 0x20 */
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;		/* 0x30 */
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;			/* 0x40 */
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint32_t SizeOfStackReserve;
    uint32_t SizeOfStackCommit;
    uint32_t SizeOfHeapReserve;		/* 0x50 */
    uint32_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    ImageDataDirectory DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; /* 0x60 */
    /* 0xE0 */
};
static_assert(sizeof(ImageOptionalHeaders32) == 224, "sizeof(Optional Header 32)");

struct ImageNtHeaders32 {
    uint32_t Signature; /* "PE"\0\0 */	/* 0x00 */
    ImageFileHeader FileHeader;		/* 0x04 */
    ImageOptionalHeaders32 OptionalHeader;	/* 0x18 */
};

enum SectionCharacteristicsAlignment {
    ALIGN_1BYTES = 0x1,
    ALIGN_2BYTES = 0x2,
    ALIGN_4BYTES = 0x3,
    ALIGN_8BYTES = 0x4,
    ALIGN_16BYTES = 0x5,
    ALIGN_32BYTES = 0x6,
    ALIGN_64BYTES = 0x7,
    ALIGN_128BYTES = 0x8,
    ALIGN_256BYTES = 0x9,
    ALIGN_512BYTES = 0xa,
    ALIGN_1024BYTES = 0xb,
    ALIGN_2048BYTES = 0xc,
    ALIGN_4096BYTES = 0xd,
    ALIGN_8192BYTES = 0xe
};

union SectionCharacteristics {
    uint32_t data;
    struct {
        uint32_t reserved0 : 3;
        uint32_t typeNoPad : 1;
        uint32_t reserved1 : 1;
        uint32_t cntCode : 1;
        uint32_t cntInitializedData : 1;
        uint32_t cntUnInitializedData : 1;
        uint32_t lnkOther : 1;
        uint32_t lnkiNFO : 1;
        uint32_t reserved2 : 1;
        uint32_t lnkRemove : 1;
        uint32_t lnkComdat : 1;
        uint32_t reserved3 : 2;
        uint32_t gprel : 1;
        uint32_t reserved4 : 1;
        uint32_t reserved5 : 3;
        uint32_t alignment : 4;
        uint32_t lnkNrelocOvfl : 1;
        uint32_t memDiscardable : 1;
        uint32_t memNotCached : 1;
        uint32_t memNotPaged : 1;
        uint32_t memShared : 1;
        uint32_t memExecute : 1;
        uint32_t memRead : 1;
        uint32_t memWrite : 1;
    } bits;
};
static_assert(sizeof(SectionCharacteristics) == 4, "sizeof(Section Characteristics)");

#define IMAGE_SIZEOF_SHORT_NAME 8

struct ImageSectionHeader {
    uint8_t Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        uint32_t PhysicalAddress;
        uint32_t VirtualSize;
    } Misc;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
};
static_assert(sizeof(ImageSectionHeader) == 40, "sizeof(Section Header)");

struct ImageExportDirectory {
    uint32_t Characteristics;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint32_t Name;
    uint32_t OrdinalBase;
    uint32_t NumberOfFunctions;
    uint32_t NumberOfNames;
    uint32_t AddressOfFunctions;
    uint32_t AddressOfNames;
    uint32_t AddressOfNameOrdinals;
};

struct ImageImportDescriptor {
    union {
        uint32_t	Characteristics; /* 0 for terminating null import descriptor  */
        uint32_t	OriginalFirstThunk;	/* RVA to original unbound IAT */
    } DUMMYUNIONNAME;
    uint32_t TimeDateStamp;	/* 0 if not bound,
				 * -1 if bound, and real date\time stamp
				 *    in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT
				 * (new BIND)
				 * otherwise date/time stamp of DLL bound to
				 * (Old BIND)
				 */
    uint32_t ForwarderChain;	/* -1 if no forwarders */
    uint32_t Name;
    /* RVA to IAT (if bound this IAT has actual addresses) */
    uint32_t FirstThunk;
};

#define IMAGE_ORDINAL_FLAG64             (((uint64_t)0x80000000 << 32) | 0x00000000)
#define IMAGE_ORDINAL_FLAG32             0x80000000

struct ImageThunkData64 {
    union {
        uint64_t ForwarderString;
        uint64_t Function;
        uint64_t Ordinal;
        uint64_t AddressOfData;
    } u1;
};

struct ImageImportByName {
    uint16_t Hint;
    uint8_t Name[1];
};

struct ImageRuntimeFunctionEntry {
    uint32_t BeginAddress;
    uint32_t EndAddress;
    union {
        uint32_t UnwindInfoAddress;
        uint32_t UnwindData;
    } DUMMYUNIONNAME;
};

enum UnwindInfoFlag {
    unw_flag_nhandler = 0,
    unw_flag_ehandler = 1,
    unw_flag_uhandler = 2,
    unw_flag_chaininfo = 4
};

enum UnwindCodeOpCode {
    uwop_push_nonvol = 0,
    uwop_alloc_large = 1,
    uwop_alloc_small = 2,
    uwop_set_fpreg = 3,
    uwop_save_nonvol = 4,
    uwop_save_nonvol_far = 5,
    uwop_epilog = 6,
    uwop_spare_code = 7,
    uwop_save_xmm128 = 8,
    uwop_save_xmm128_far = 9,
    uwop_push_machframe = 10,
};

enum UnwindCodeOpInfo {
    uwinfo_rax = 0,
    uwinfo_rcx,
    uwinfo_rdx,
    uwinfo_rbx,
    uwinfo_rsp,
    uwinfo_rbp,
    uwinfo_rsi,
    uwinfo_rdi,
    uwinfo_r8,
    uwinfo_r9,
    uwinfo_r10,
    uwinfo_r11,
    uwinfo_r12,
    uwinfo_r13,
    uwinfo_r14,
    uwinfo_r15,
};

union UnwindCode {
    struct {
        uint16_t Offset : 8;
        uint16_t OpCode : 4;
        uint16_t OpInfo : 4;
    } u;
    uint16_t FrameOffset;
};
static_assert(sizeof(UnwindCode) == 2, "sizeof(UnwindCode)");

struct UnwindInfo {
    uint8_t Version : 3;
    uint8_t Flags : 5;
    uint8_t SizeOfProlog; // size of the function prolog in bytes
    uint8_t CountOfCodes;
    uint8_t FrameRegister : 4; // UnwindCodeOpInfo of which register serves as the frame pointer (if non zero)
    uint8_t FrameOffset : 4; // actual frame pointer = frame pointer value + 16 * this offset
};

enum DebugType {
    image_debug_type_unknown = 0,
    image_debug_type_coff = 1,
    image_debug_type_codeview = 2,
    image_debug_type_fpo = 3,
    image_debug_type_misc = 4,
    image_debug_type_exception = 5,
    image_debug_type_fixup = 6,
    image_debug_type_omap_to_src = 7,
    image_debug_type_omap_from_src = 8,
    image_debug_type_borland = 9,
    image_debug_type_reserved10 = 10,
    image_debug_type_clsid = 11,
    image_debug_type_repro = 16,
    image_debug_type_undefined1 = 17,
    image_debug_type_undefined2 = 18,
    image_debug_type_ex_dllcharacteristics = 20,
};

struct ImageDebugDirectory {
    uint32_t Characteristics;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint32_t Type;
    uint32_t SizeOfData;
    uint32_t AddressOfRawData;
    uint32_t PointerToRawData;
};

struct FpoData {
    uint32_t OffStart;
    uint32_t ProcSize;
    uint32_t Locals;
    uint16_t Params;
    uint16_t Prolog : 8;
    uint16_t Regs : 3;
    uint16_t HasSeh : 1;
    uint16_t UseBP : 1;
    uint16_t Reserved1 : 1;
    uint16_t Frame : 2;
};

#pragma pack(pop)

enum class memory_alignment {
    file,
    loaded
};


static const char* str_subsystem(const Subsystem subsystem) {
    switch (subsystem) {
        case subsystem_unknown: return "Unknown";
        case subsystem_native: return "Native";
        case subsystem_windows_gui: return "WindowsGui";
        case subsystem_windows_cui: return "WindowsCui";
        case subsystem_os2_cui: return "Os2Cui";
        case subsystem_posix_cui: return "PosixCui";
        case subsystem_native_windows: return "NativeWindows";
        case subsystem_windows_ce_gui: return "WindowsCeGui";
        case subsystem_efi_application: return "EfiApplication";
        case subsystem_efi_boot_service_driver: return "EfiBootServiceDriver";
        case subsystem_efi_runtime_driver: return "EfiRuntimeDriver";
        case subsystem_efi_rom: return "EfiRom";
        case subsystem_xbox: return "Xbox";
        case subsystem_windows_boot_application: return "WindowsBootApplication";
        default: return "";
    }
}

static const char* str_machine_type(const MachineType type) {
    switch (type) {
        case machine_unknown: return "Unknown";
        case machine_i860: return "I860";
        case machine_i386: return "I386";
        case machine_r3000: return "R3000";
        case machine_r4000: return "R4000";
        case machine_r10000: return "R10000";
        case machine_wcemipsv2: return "WCEMIPSv2";
        case machine_alpha: return "Alpha";
        case machine_sh3: return "Sh3";
        case machine_sh3dsp: return "Sh3DSP";
        case machine_sh3e: return "Sh3e";
        case machine_sh4: return "Sh4";
        case machine_sh5: return "Sh5";
        case machine_arm: return "Arm";
        case machine_thumb: return "Thumb";
        case machine_armnt: return "ArmNT";
        case machine_am33: return "Am33";
        case machine_powerpc: return "PowerPC";
        case machine_powerpcfp: return "PowerPCFP";
        case machine_ia64: return "IA64";
        case machine_mips16: return "Mips16";
        case machine_alpha64: return "Alpha64";
        case machine_mipsfpu: return "MipsFPU";
        case machine_mipsfpu16: return "MipsFPU16";
        case machine_tricore: return "Tricore";
        case machine_cef: return "CEF";
        case machine_ebc: return "EBC";
        case machine_amd64: return "AMD64";
        case machine_m32r: return "M32r";
        case machine_cee: return "CEE";
        case machine_arm64: return "ARM64";
        default: return "";
    }
}

static const char* str_data_directory_type(const DataDirectoryType type) {
    switch (type) {
        case image_directory_entry_export: return "Export";
        case image_directory_entry_import: return "Import";
        case image_directory_entry_resource: return "Resource";
        case image_directory_entry_exception: return "Exception";
        case image_directory_entry_security: return "Security";
        case image_directory_entry_basereloc: return "BaseReloc";
        case image_directory_entry_debug: return "Debug";
        case image_directory_entry_copyright: return "Copyright";
        case image_directory_entry_globalptr: return "GlobalPtr";
        case image_directory_entry_tls: return "Tls";
        case image_directory_entry_load_config: return "LoadConfig";
        case image_directory_entry_bound_import: return "BoundImport";
        case image_directory_entry_iat: return "IAT";
        case image_directory_entry_delay_import: return "DelayImport";
        case image_directory_entry_com_descriptor: return "ComDescriptor";
        case image_directory_max:
        default: return "";
    }
}

static const char* str_unwind_code_op_code(const UnwindCodeOpCode code) {
    switch (code) {
        case uwop_push_nonvol: return "Push NonVol";
        case uwop_alloc_large: return "Alloc Large";
        case uwop_alloc_small: return "Alloc Small";
        case uwop_set_fpreg: return "Set FPREG";
        case uwop_save_nonvol: return "Save NonVol";
        case uwop_save_nonvol_far: return "Save NonVol Far";
        case uwop_epilog: return "Epilog";
        case uwop_spare_code: return "Spare Code";
        case uwop_save_xmm128: return "Save XMM128";
        case uwop_save_xmm128_far: return "Save XMM128 Far";
        case uwop_push_machframe: return "Push MachFrame";
        default: return "";
    }
}

static const char* str_unwind_code_op_info_register(const UnwindCodeOpInfo info) {
    switch (info) {
        case uwinfo_rax: return "rax";
        case uwinfo_rcx: return "rcx";
        case uwinfo_rdx: return "rdx";
        case uwinfo_rbx: return "rbx";
        case uwinfo_rsp: return "rsp";
        case uwinfo_rbp: return "rbp";
        case uwinfo_rsi: return "rsi";
        case uwinfo_rdi: return "rdi";
        case uwinfo_r8: return "r8";
        case uwinfo_r9: return "r9";
        case uwinfo_r10: return "r10";
        case uwinfo_r11: return "r11";
        case uwinfo_r12: return "r12";
        case uwinfo_r13: return "r13";
        case uwinfo_r14: return "r14";
        case uwinfo_r15: return "r15";
        default: return "";
    }
}

}

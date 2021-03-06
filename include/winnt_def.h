#pragma once

#include <cstdint>


namespace pe {

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
    MACHINE_UNKNOWN	= 0,
    MACHINE_I860 = 0x014d,
    MACHINE_I386 = 0x014c,
    MACHINE_R3000 = 0x0162,
    MACHINE_R4000 = 0x0166,
    MACHINE_R10000 = 0x0168,
    MACHINE_WCEMIPSV2 = 0x0169,
    MACHINE_ALPHA = 0x0184,
    MACHINE_SH3 = 0x01a2,
    MACHINE_SH3DSP = 0x01a3,
    MACHINE_SH3E = 0x01a4,
    MACHINE_SH4 = 0x01a6,
    MACHINE_SH5 = 0x01a8,
    MACHINE_ARM = 0x01c0,
    MACHINE_THUMB = 0x01c2,
    MACHINE_ARMNT = 0x01c4,
    MACHINE_AM33 = 0x01d3,
    MACHINE_POWERPC	= 0x01f0,
    MACHINE_POWERPCFP = 0x01f1,
    MACHINE_IA64 = 0x0200,
    MACHINE_MIPS16 = 0x0266,
    MACHINE_ALPHA64	= 0x0284,
    MACHINE_MIPSFPU	= 0x0366,
    MACHINE_MIPSFPU16 = 0x0466,
    MACHINE_AXP64 = MACHINE_ALPHA64,
    MACHINE_TRICORE	= 0x0520,
    MACHINE_CEF	= 0x0cef,
    MACHINE_EBC	= 0x0ebc,
    MACHINE_AMD64 = 0x8664,
    MACHINE_M32R = 0x9041,
    MACHINE_CEE	= 0xc0ee,
    MACHINE_ARM64 = 0x01c5
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
    IMAGE_DIRECTORY_ENTRY_EXPORT = 0,
    IMAGE_DIRECTORY_ENTRY_IMPORT = 1,
    IMAGE_DIRECTORY_ENTRY_RESOURCE = 2,
    IMAGE_DIRECTORY_ENTRY_EXCEPTION = 3,
    IMAGE_DIRECTORY_ENTRY_SECURITY = 4,
    IMAGE_DIRECTORY_ENTRY_BASERELOC = 5,
    IMAGE_DIRECTORY_ENTRY_DEBUG = 6,
    IMAGE_DIRECTORY_ENTRY_COPYRIGHT = 7,
    IMAGE_DIRECTORY_ENTRY_GLOBALPTR = 8,   /* (MIPS GP) */
    IMAGE_DIRECTORY_ENTRY_TLS = 9,
    IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG = 10,
    IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT = 11,
    IMAGE_DIRECTORY_ENTRY_IAT = 12,  /* Import Address Table */
    IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT = 13,
    IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR = 14, 
};

enum Subsystem {
    SUBSYSTEM_UNKNOWN = 0,
    SUBSYSTEM_NATIVE = 1,
    SUBSYSTEM_WINDOWS_GUI = 2,	/* Windows GUI subsystem */
    SUBSYSTEM_WINDOWS_CUI = 3,	/* Windows character subsystem */
    SUBSYSTEM_OS2_CUI = 5,
    SUBSYSTEM_POSIX_CUI = 7,
    SUBSYSTEM_NATIVE_WINDOWS = 8,	/* native Win9x driver */
    SUBSYSTEM_WINDOWS_CE_GUI = 9,	/* Windows CE subsystem */
    SUBSYSTEM_EFI_APPLICATION = 10,
    SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER = 11,
    SUBSYSTEM_EFI_RUNTIME_DRIVER = 12,
    SUBSYSTEM_EFI_ROM = 13,
    SUBSYSTEM_XBOX = 14,
    SUBSYSTEM_WINDOWS_BOOT_APPLICATION = 16,
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

#pragma pack(pop)

}

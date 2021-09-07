
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

#include "pe.h"
#include "except.h"
#include "debug.h"


/* These defines are for the Characteristics bitfield. */
/* #define IMAGE_SCN_TYPE_REG			0x00000000 - Reserved */
/* #define IMAGE_SCN_TYPE_DSECT			0x00000001 - Reserved */
/* #define IMAGE_SCN_TYPE_NOLOAD		0x00000002 - Reserved */
/* #define IMAGE_SCN_TYPE_GROUP			0x00000004 - Reserved */
#define IMAGE_SCN_TYPE_NO_PAD			0x00000008 /* Reserved */
/* #define IMAGE_SCN_TYPE_COPY			0x00000010 - Reserved */

#define IMAGE_SCN_CNT_CODE			0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA		0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA	0x00000080

#define	IMAGE_SCN_LNK_OTHER			0x00000100
#define	IMAGE_SCN_LNK_INFO			0x00000200
/* #define	IMAGE_SCN_TYPE_OVER		0x00000400 - Reserved */
#define	IMAGE_SCN_LNK_REMOVE			0x00000800
#define	IMAGE_SCN_LNK_COMDAT			0x00001000

/* 						0x00002000 - Reserved */
/* #define IMAGE_SCN_MEM_PROTECTED 		0x00004000 - Obsolete */
#define	IMAGE_SCN_MEM_FARDATA			0x00008000

/* #define IMAGE_SCN_MEM_SYSHEAP		0x00010000 - Obsolete */
#define	IMAGE_SCN_MEM_PURGEABLE			0x00020000
#define	IMAGE_SCN_MEM_16BIT			0x00020000
#define	IMAGE_SCN_MEM_LOCKED			0x00040000
#define	IMAGE_SCN_MEM_PRELOAD			0x00080000

#define	IMAGE_SCN_ALIGN_1BYTES			0x00100000
#define	IMAGE_SCN_ALIGN_2BYTES			0x00200000
#define	IMAGE_SCN_ALIGN_4BYTES			0x00300000
#define	IMAGE_SCN_ALIGN_8BYTES			0x00400000
#define	IMAGE_SCN_ALIGN_16BYTES			0x00500000  /* Default */
#define	IMAGE_SCN_ALIGN_32BYTES			0x00600000
#define	IMAGE_SCN_ALIGN_64BYTES			0x00700000
#define	IMAGE_SCN_ALIGN_128BYTES		0x00800000
#define	IMAGE_SCN_ALIGN_256BYTES		0x00900000
#define	IMAGE_SCN_ALIGN_512BYTES		0x00A00000
#define	IMAGE_SCN_ALIGN_1024BYTES		0x00B00000
#define	IMAGE_SCN_ALIGN_2048BYTES		0x00C00000
#define	IMAGE_SCN_ALIGN_4096BYTES		0x00D00000
#define	IMAGE_SCN_ALIGN_8192BYTES		0x00E00000
/* 						0x00F00000 - Unused */
#define	IMAGE_SCN_ALIGN_MASK			0x00F00000

#define IMAGE_SCN_LNK_NRELOC_OVFL		0x01000000


#define IMAGE_SCN_MEM_DISCARDABLE		0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED		0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED			0x08000000
#define IMAGE_SCN_MEM_SHARED			0x10000000
#define IMAGE_SCN_MEM_EXECUTE			0x20000000
#define IMAGE_SCN_MEM_READ			0x40000000
#define IMAGE_SCN_MEM_WRITE			0x80000000

int main() {
    std::ifstream file("/home/tomtzook/vm/shared/ntoskrnl.exe", std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    try {
        pe::Image image(buffer.data());
        std::cout << image << std::endl;

        for (const auto& section : image.sections()) {
            std::cout << section << std::endl;
        }

        {
            /*auto exportTable = image.exportTable();

            std::cout << "ADDRESS TABLE" << std::endl;
            for(const auto& addressEntry : exportTable) {
                std::cout << " rva=" << addressEntry.rva()
                          << ", ordinal=" << addressEntry.ordinal()
                          << ", baised=" << addressEntry.baisedOrdinal()
                          << ", ptr=" << reinterpret_cast<const void*>(image.rvaToPointer<uint8_t>(addressEntry.rva()));
                          //<< std::endl;

                try {
                    auto nameEntry = exportTable.names()[addressEntry.ordinal()];
                    std::cout << " name=" << nameEntry.name();
                } catch (const pe::OrdinalNotFoundException& ex) {
                    // no name for export
                }

                std::cout << std::endl;
            }*/

            /*std::cout << "NAME TABLE" << std::endl;
            for(const auto& aentry : exportTable.names()) {
                std::cout << " name=" << aentry.name()
                          << ", ordinal=" << aentry.ordinal()
                          << ", baised=" << aentry.baisedOrdinal()
                          << std::endl;
            }*/

            /*{
                auto entry = exportTable.names()["ExAllocatePoolWithTag"];
                pe::rva_t rva = exportTable[entry.ordinal()];
                auto ptr = image.rvaToPointer<uint8_t>(rva);
                std::cout << entry << " ptr=" << reinterpret_cast<const void*>(ptr) << std::endl;
            }

            {
                pe::export_ordinal_t ordinal = 0xa;
                pe::rva_t rva = exportTable[ordinal];
                auto entry = exportTable.names()[ordinal];
                auto ptr = image.rvaToPointer<uint8_t>(rva);
                std::cout << entry << " ptr=" << reinterpret_cast<const void*>(ptr) << std::endl;
            }*/
        }

    } catch (const pe::BadHeaderException& ex) {
        printf("Bad Header: %d\n", ex.reason());
    }

    return 0;
}

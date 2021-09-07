
#include "except.h"
#include "import.h"


namespace pe {

ImportedModule::entry::entry(const entry& entry)
    : m_thunkData(entry.m_thunkData)
    , m_section(entry.m_section)
{}

ImportedModule::entry& ImportedModule::entry::operator=(const entry& other) {
    m_thunkData = other.m_thunkData;
    return *this;
}

bool ImportedModule::entry::is_ordinal() const {
    return m_thunkData->u1.AddressOfData & IMAGE_ORDINAL_FLAG64;
}

uint16_t ImportedModule::entry::ordinal() const {
    return static_cast<uint16_t>(m_thunkData->u1.AddressOfData & ~IMAGE_ORDINAL_FLAG64);
}

bool ImportedModule::entry::is_name() const {
    return !is_ordinal() && m_section.containsRva(m_thunkData->u1.AddressOfData);
}

const char* ImportedModule::entry::name() const {
    auto importByName = m_section.rvaToPointer<ImageImportByName>(m_thunkData->u1.AddressOfData);
    return reinterpret_cast<const char*>(importByName->Name);
}

ImportedModule::entry::entry(const ImageThunkData64* thunkData, const Section& section)
    : m_thunkData(thunkData)
    , m_section(section)
{}

ImportedModule::iterator::iterator(const ImageThunkData64* thunkData, const Section& section)
    : m_thunkData(thunkData)
    , m_section(section)
{}

ImportedModule::iterator& ImportedModule::iterator::operator++() {
    m_thunkData++;
    return *this;
}

ImportedModule::iterator& ImportedModule::iterator::operator--() {
    m_thunkData--;
    return *this;
}

ImportedModule::iterator::reference ImportedModule::iterator::operator*() {
    return {m_thunkData, m_section};
}

ImportedModule::iterator::pointer ImportedModule::iterator::operator->() {
    return {m_thunkData, m_section};
}

bool ImportedModule::iterator::operator==(const iterator& rhs) {
    return m_thunkData == rhs.m_thunkData;
}

bool ImportedModule::iterator::operator!=(const iterator& rhs) {
    return m_thunkData != rhs.m_thunkData;
}

ImportedModule::ImportedModule(const ImageImportDescriptor* descriptor, const Section& section)
    : m_descriptor(descriptor)
    , m_section(section)
{}

const char* ImportedModule::name() const {
    if (m_descriptor->Name) {
        return m_section.rvaToPointer<char>(m_descriptor->Name);
    }

    return "";
}

ImportedModule::iterator ImportedModule::begin() const {
    auto thunkData = m_section.rvaToPointer<ImageThunkData64>(m_descriptor->FirstThunk);
    return iterator(thunkData, m_section);
}

ImportedModule::iterator ImportedModule::end() const {
    auto thunkData = m_section.rvaToPointer<ImageThunkData64>(m_descriptor->FirstThunk);
    return iterator(thunkData + findLastDataIndex(), m_section);
}

size_t ImportedModule::findLastDataIndex() const {
    auto thunkData = m_section.rvaToPointer<ImageThunkData64>(m_descriptor->FirstThunk);
    size_t i = 0;
    for (; thunkData[i].u1.AddressOfData; i++);

    return i;
}

ImportTable::iterator::iterator(const ImageImportDescriptor* importDescriptor, const Section& section)
    : m_importDescriptor(importDescriptor)
    , m_section(section)
{}

ImportTable::iterator& ImportTable::iterator::operator++() {
    m_importDescriptor++;
    return *this;
}

ImportTable::iterator& ImportTable::iterator::operator--() {
    m_importDescriptor--;
    return *this;
}

ImportTable::iterator::reference ImportTable::iterator::operator*() {
    return {m_importDescriptor, m_section};
}

ImportTable::iterator::pointer ImportTable::iterator::operator->() {
    return {m_importDescriptor, m_section};
}

bool ImportTable::iterator::operator==(const iterator& rhs) {
    return m_importDescriptor == rhs.m_importDescriptor;
}

bool ImportTable::iterator::operator!=(const iterator& rhs) {
    return m_importDescriptor != rhs.m_importDescriptor;
}

ImportTable::ImportTable(const ImageImportDescriptor* importDescriptors, Section section)
    : m_importDescriptors(importDescriptors)
    , m_section(section)
{}

ImportTable::iterator ImportTable::begin() const {
    return iterator(m_importDescriptors, m_section);
}

ImportTable::iterator ImportTable::end() const {
    return iterator(m_importDescriptors + findLastDataIndex(), m_section);
}

size_t ImportTable::findLastDataIndex() const {
    size_t i = 0;
    for (; m_importDescriptors[i].FirstThunk; i++);

    return i;
}

}
/*
 * ImportTable::ImportTable(const void* importDirectoryPtr, const PeHeaders& headers, Section section)
    : m_section(section) {
    auto descriptor = reinterpret_cast<const ImageImportDescriptor*>(importDirectoryPtr);
    for (; descriptor->FirstThunk; descriptor++) {
        printf("Descriptor:\n");
        printf("\tFirstThunk=0x%x\n", descriptor->FirstThunk);
        printf("\tName RVA=0x%x\n", descriptor->Name);
        if (descriptor->Name) {
            auto name = section.rvaToPointer<char>(descriptor->Name);
            printf("\tName=%s\n", name);
        }

        auto thunkData = section.rvaToPointer<ImageThunkData64>(descriptor->FirstThunk);
        for (; thunkData->u1.AddressOfData; thunkData++) {
            auto data = thunkData->u1.AddressOfData;
            if (data & IMAGE_ORDINAL_FLAG64) {
                // import by ordinal
                printf("\t\tOrdinal: 0x%lx\n", (data & ~IMAGE_ORDINAL_FLAG64));
            } else {
                // import by name
                try {
                    auto importByName = section.rvaToPointer<ImageImportByName>(data);
                    printf("\t\tName: %s\n", importByName->Name);
                } catch (const pe::RvaNotInSectionException& ex) {
                    printf("\t\tName: 0x%x (0x%lx) (0x%x 0x%x)\n",
                           ex.rva(), data, headers.base(), headers.size());
                }
            }
        }
    }
}
 */
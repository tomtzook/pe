#include <fstream>
#include <sstream>
#include <format>
#include <imgui/imgui.h>

#include "pe_display.h"


template<typename t>
std::string mask_str(const t mask, const char*(*bit_to_str)(t)) {
    std::stringstream ss;
    bool has_one = false;
    for (int i = 0; i < sizeof(mask) * 8; i++) {
        if (const auto bit = mask & (1 << i)) {
            const auto str = bit_to_str(bit);

            if (has_one) {
                ss << '|';
            } else {
                has_one = true;
            }

            ss << str;
        }
    }

    return ss.str();
}

static std::vector<char> load_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::end);
    const size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(file_size);
    file.read(buffer.data(), file_size);

    return std::move(buffer);
}

static std::string get_filename(const std::string& path) {
    const auto last_dot_pos = path.find_last_of('/');
    if (last_dot_pos == std::string::npos || last_dot_pos == 0) {
        return path;
    }

    return path.substr(last_dot_pos + 1);
}

static const char* data_directory_type_str(const pe::DataDirectoryType type) {
    switch (type) {
        case pe::IMAGE_DIRECTORY_ENTRY_EXPORT: return "Export";
        case pe::IMAGE_DIRECTORY_ENTRY_IMPORT: return "Import";
        case pe::IMAGE_DIRECTORY_ENTRY_RESOURCE: return "Resource";
        case pe::IMAGE_DIRECTORY_ENTRY_EXCEPTION: return "Exception";
        case pe::IMAGE_DIRECTORY_ENTRY_SECURITY: return "Security";
        case pe::IMAGE_DIRECTORY_ENTRY_BASERELOC: return "BaseReloc";
        case pe::IMAGE_DIRECTORY_ENTRY_DEBUG: return "Debug";
        case pe::IMAGE_DIRECTORY_ENTRY_COPYRIGHT: return "Copyright";
        case pe::IMAGE_DIRECTORY_ENTRY_GLOBALPTR: return "GlobalPtr";
        case pe::IMAGE_DIRECTORY_ENTRY_TLS: return "Tls";
        case pe::IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG: return "LoadConfig";
        case pe::IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT: return "BoundImport";
        case pe::IMAGE_DIRECTORY_ENTRY_IAT: return "IAT";
        case pe::IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT: return "DelayImport";
        case pe::IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR: return "ComDescriptor";
        case pe::IMAGE_DIRECTORY_MAX:
        default:
            return "";
    }
}

static const char* frame_register_str(const uint32_t frame_register) {
    switch (static_cast<pe::UnwindCodeOpInfo>(frame_register)) {
        case pe::UWINFO_RAX: return "rax";
        case pe::UWINFO_RCX: return "rcx";
        case pe::UWINFO_RDX: return "rdx";
        case pe::UWINFO_RBX: return "rbx";
        case pe::UWINFO_RSP: return "rsp";
        case pe::UWINFO_RBP: return "rbp";
        case pe::UWINFO_RSI: return "rsi";
        case pe::UWINFO_RDI: return "rdi";
        case pe::UWINFO_R8: return "r8";
        case pe::UWINFO_R9: return "r9";
        case pe::UWINFO_R10: return "r10";
        case pe::UWINFO_R11: return "r11";
        case pe::UWINFO_R12: return "r12";
        case pe::UWINFO_R13: return "r13";
        case pe::UWINFO_R14: return "r14";
        case pe::UWINFO_R15: return "r15";
        default: return "";
    }
}

tree_leaf::tree_leaf(const std::string_view name)
    : m_name(name)
    , m_value_type(type::empty)
    , m_value()
{}

std::string_view tree_leaf::get_name() const {
    return m_name;
}

bool tree_leaf::has_value() const {
    return m_value_type != type::empty;
}

tree_leaf::type tree_leaf::get_value_type() const {
    return m_value_type;
}

bool tree_leaf::get_value_bool() const {
    assert(m_value_type == type::boolean);
    return m_value.b;
}

int32_t tree_leaf::get_value_int() const {
    assert(m_value_type == type::integer);
    return m_value.i;
}

uint32_t tree_leaf::get_value_uint() const {
    assert(m_value_type == type::uinteger);
    return m_value.u;
}

float tree_leaf::get_value_float() const {
    assert(m_value_type == type::floating);
    return m_value.f;
}

const char* tree_leaf::get_value_str() const {
    assert(m_value_type == type::string);
    return m_value.s;
}

void tree_leaf::set_value_bool(const bool value) {
    m_value_type = type::boolean;
    m_value.b = value;
}

void tree_leaf::set_value_int(const int32_t value) {
    m_value_type = type::integer;
    m_value.i = value;
}

void tree_leaf::set_value_uint(const uint32_t value) {
    m_value_type = type::uinteger;
    m_value.u = value;
}

void tree_leaf::set_value_float(const float value) {
    m_value_type = type::floating;
    m_value.f = value;
}

void tree_leaf::set_value_str(const char* value) {
    m_value_type = type::string;
    m_value.s = value;
}

void tree_leaf::draw() const {
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", m_name.data());
    ImGui::TableSetColumnIndex(1);
    switch (m_value_type) {
        case type::boolean:
            ImGui::Text("%s", get_value_bool() ? "true" : "false");
            break;
        case type::integer:
            ImGui::Text("%d", get_value_int());
            break;
        case type::uinteger:
            ImGui::Text("0x%x", get_value_uint());
            break;
        case type::floating:
            ImGui::Text("%.3f", get_value_float());
            break;
        case type::string:
            ImGui::Text("%s", get_value_str());
            break;
        case type::empty:
        default:
            break;
    }
}

tree_node::tree_node()
    : m_name()
    , m_children()
    , m_leafs()
{}

tree_node::tree_node(const std::string_view name)
    : m_name(name)
    , m_children()
    , m_leafs()
{}

std::string_view tree_node::get_name() const {
    return m_name;
}

void tree_node::set_name(const std::string_view name) {
    m_name = name;
}

bool tree_node::has_children() const {
    return !m_children.empty();
}

size_t tree_node::get_children_count() const {
    return m_children.size();
}

void tree_node::add_child(tree_node&& child) {
    m_children.emplace_back(std::move(child));
}

void tree_node::add_leaf_bool(const std::string_view name, const bool value) {
    tree_leaf leaf(name);
    leaf.set_value_bool(value);
    m_leafs.emplace_back(std::move(leaf));
}

void tree_node::add_leaf_int(const std::string_view name, const int32_t value) {
    tree_leaf leaf(name);
    leaf.set_value_int(value);
    m_leafs.emplace_back(std::move(leaf));
}

void tree_node::add_leaf_uint(const std::string_view name, const uint32_t value) {
    tree_leaf leaf(name);
    leaf.set_value_uint(value);
    m_leafs.emplace_back(std::move(leaf));
}

void tree_node::add_leaf_float(const std::string_view name, const float value) {
    tree_leaf leaf(name);
    leaf.set_value_float(value);
    m_leafs.emplace_back(std::move(leaf));
}

void tree_node::add_leaf_str(const std::string_view name, const char* value) {
    tree_leaf leaf(name);
    leaf.set_value_str(value);
    m_leafs.emplace_back(std::move(leaf));
}

void tree_node::draw() const {
    if (ImGui::TreeNode(m_name.c_str())) {
        for (const auto& leaf : m_leafs) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            leaf.draw();
        }

        for (const auto& child : m_children) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            child.draw();
        }

        ImGui::TreePop();
    }
}

pe_display::pe_display(const std::string_view path)
    : m_path(path)
    , m_name(get_filename(m_path))
    , m_data(load_file(m_path))
    , m_image(m_data.data())
    , m_information_root(load_info_tree())
{}

std::string_view pe_display::get_path() const {
    return m_path;
}

std::string_view pe_display::get_name() const {
    return m_name;
}

void pe_display::display() const {
    ImGui::Begin(m_name.c_str());
    if (ImGui::BeginTable("Info", 2, ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        m_information_root.draw();

        ImGui::EndTable();
    }
    ImGui::End();
}

tree_node pe_display::load_info_tree() const {
    tree_node root(m_name);

    {
        const auto characteristics = m_image.headers().characteristics();
        tree_node node("Characteristics");
        node.add_leaf_bool("RelocsStripped", characteristics.bits.RelocsStripped);
        node.add_leaf_bool("ExecutableImage", characteristics.bits.ExecutableImage);
        node.add_leaf_bool("LineNumsStripped", characteristics.bits.LineNumsStripped);
        node.add_leaf_bool("LocalSymsStripped", characteristics.bits.LocalSymsStripped);
        node.add_leaf_bool("AggressiveWsTrim", characteristics.bits.AggressiveWsTrim);
        node.add_leaf_bool("LargeAddressAware", characteristics.bits.LargeAddressAware);
        node.add_leaf_bool("BytesReversedLo", characteristics.bits.BytesReversedLo);
        node.add_leaf_bool("bit32Machine", characteristics.bits.bit32Machine);
        node.add_leaf_bool("DebugStripped", characteristics.bits.DebugStripped);
        node.add_leaf_bool("RemovableRunFromSwap", characteristics.bits.RemovableRunFromSwap);
        node.add_leaf_bool("NetRunFromSwap", characteristics.bits.NetRunFromSwap);
        node.add_leaf_bool("FileSystem", characteristics.bits.FileSystem);
        node.add_leaf_bool("Dll", characteristics.bits.Dll);
        node.add_leaf_bool("UpSystemOnly", characteristics.bits.UpSystemOnly);
        node.add_leaf_bool("BytesReversedHi", characteristics.bits.BytesReversedHi);

        root.add_child(std::move(node));
    }

    {
        const auto characteristics = m_image.headers().dllCharacteristics();
        tree_node node("DllCharacteristics");
        node.add_leaf_bool("HighEntropyVa", characteristics.bits.HighEntropyVa);
        node.add_leaf_bool("DynamicBase", characteristics.bits.DynamicBase);
        node.add_leaf_bool("ForceIntegrity", characteristics.bits.ForceIntegrity);
        node.add_leaf_bool("NxCompat", characteristics.bits.NxCompat);
        node.add_leaf_bool("NoIsolation", characteristics.bits.NoIsolation);
        node.add_leaf_bool("NoSeh", characteristics.bits.NoSeh);
        node.add_leaf_bool("NoBind", characteristics.bits.NoBind);
        node.add_leaf_bool("AppContainer", characteristics.bits.AppContainer);
        node.add_leaf_bool("WdmDriver", characteristics.bits.WdmDriver);
        node.add_leaf_bool("GuardCf", characteristics.bits.GuardCf);
        node.add_leaf_bool("TerminalServerAware", characteristics.bits.TerminalServerAware);

        root.add_child(std::move(node));
    }

    root.add_child(load_sections());
    root.add_child(load_data_directories());
    root.add_child(load_exports());
    root.add_child(load_imports());
    root.add_child(load_exceptions());

    return std::move(root);
}

tree_node pe_display::load_sections() const {
    tree_node root("Sections");
    for (const auto& section : m_image.sections()) {
        tree_node section_node(section.name());
        section_node.add_leaf_uint("VirtualAddress", section.virtual_address());
        section_node.add_leaf_uint("VirtualSize", section.virtual_size());
        section_node.add_leaf_uint("PointerToRawData", section.pointer_to_raw_data());
        section_node.add_leaf_uint("SizeOfRawData", section.size_of_raw_data());

        {
            const auto characteristics = section.characteristics();
            tree_node node("Characteristics");
            node.add_leaf_bool("TypeNoPad", characteristics.bits.typeNoPad);
            node.add_leaf_bool("CntCode", characteristics.bits.cntCode);
            node.add_leaf_bool("CntInitializedData", characteristics.bits.cntInitializedData);
            node.add_leaf_bool("CntUnInitializedData", characteristics.bits.cntUnInitializedData);
            node.add_leaf_bool("LnkOther", characteristics.bits.lnkOther);
            node.add_leaf_bool("LnkiNFO", characteristics.bits.lnkiNFO);
            node.add_leaf_bool("LnkRemove", characteristics.bits.lnkRemove);
            node.add_leaf_bool("LnkComdat", characteristics.bits.lnkComdat);
            node.add_leaf_bool("Gprel", characteristics.bits.gprel);
            node.add_leaf_uint("Alignment", characteristics.bits.alignment); //todo: string of SectionCharacteristicsAlignment
            node.add_leaf_bool("LnkNrelocOvfl", characteristics.bits.lnkNrelocOvfl);
            node.add_leaf_bool("MemDiscardable", characteristics.bits.memDiscardable);
            node.add_leaf_bool("MemNotCached", characteristics.bits.memNotCached);
            node.add_leaf_bool("MemNotPaged", characteristics.bits.memNotPaged);
            node.add_leaf_bool("MemShared", characteristics.bits.memShared);
            node.add_leaf_bool("MemExecute", characteristics.bits.memExecute);
            node.add_leaf_bool("MemRead", characteristics.bits.memRead);
            node.add_leaf_bool("MemWrite", characteristics.bits.memWrite);

            section_node.add_child(std::move(node));
        }

        root.add_child(std::move(section_node));
    }

    return std::move(root);
}

tree_node pe_display::load_data_directories() const {
    tree_node root("DataDirectories");

    for (auto type = pe::DataDirectoryType::IMAGE_DIRECTORY_ENTRY_EXPORT;
        type < pe::DataDirectoryType::IMAGE_DIRECTORY_MAX;
        type = static_cast<pe::DataDirectoryType>(static_cast<uint32_t>(type) + 1)) {
        tree_node node(data_directory_type_str(type));

        const auto directory = m_image.headers().data_directory(type);
        if (directory == nullptr) {
            continue;
        }

        node.add_leaf_uint("VirtualAddress", directory->VirtualAddress);
        node.add_leaf_uint("Size", directory->Size);

        root.add_child(std::move(node));
    }

    return std::move(root);
}

tree_node pe_display::load_exports() const {
    tree_node root("Exports");
    const auto export_table = m_image.load_export_table();
    if (!export_table.is_valid()) {
        return std::move(root);
    }

    const auto export_names = export_table.names();
    for (const auto& entry : export_table) {
        tree_node node;

        node.add_leaf_uint("Rva", entry.rva());
        node.add_leaf_uint("Ordinal", entry.ordinal());

        if (const auto name_entry = export_names[entry.ordinal()]; name_entry.is_valid()) {
            node.set_name(name_entry.name());
        } else {
            node.set_name(std::format("0x{:x}", entry.ordinal()));
        }
    }

    return std::move(root);
}

tree_node pe_display::load_imports() const {
    tree_node root("Imports");
    const auto import_table = m_image.load_import_table();
    if (!import_table.is_valid()) {
        return std::move(root);
    }

    for (const auto& module : import_table) {
        tree_node module_node(module.name());

        for (const auto& entry : module) {
            tree_node entry_node(entry.name());
            entry_node.add_leaf_uint("Ordinal", entry.ordinal());

            module_node.add_child(std::move(entry_node));
        }

        root.add_child(std::move(module_node));
    }

    return std::move(root);
}

tree_node pe_display::load_exceptions() const {
    tree_node root("Exceptions");
    const auto exception_table = m_image.load_exception_table();
    if (!exception_table.is_valid()) {
        return std::move(root);
    }

    for (const auto& entry : exception_table) {
        tree_node entry_node(std::format("0x{:x} -> 0x{:x}", entry.start(), entry.end()));

        entry_node.add_leaf_uint("StartRva", entry.start());
        entry_node.add_leaf_uint("EndRva", entry.end());
        entry_node.add_leaf_uint("UnwindInfoRva", entry.unwind_info_rva());

        if (auto unwind_info = entry.find_unwind_info(m_image.sections());
            unwind_info.is_valid()) {
            int index = 0;
            do {
                const auto name = index > 0 ? std::format("UnwindInfo {}", index + 1) : "UnwindInfo";
                tree_node unwind_node(name);

                unwind_node.add_leaf_uint("Flags", unwind_info.flags());
                unwind_node.add_leaf_uint("PrologSize", unwind_info.prolog_size());
                unwind_node.add_leaf_str("FrameRegister", frame_register_str(unwind_info.frame_register()));
                unwind_node.add_leaf_uint("FrameRegisterOffset", unwind_info.frame_register_offset() * 16);
                entry_node.add_child(std::move(unwind_node));

                unwind_info = unwind_info.next();
                index++;
            } while (unwind_info.is_valid());
        }

        root.add_child(std::move(entry_node));
    }

    return std::move(root);
}

bool pe_displays_container::is_loaded() const {
    return m_pe.has_value();
}

std::string_view pe_displays_container::get_loaded_path() const {
    if (m_pe.has_value()) {
        return m_pe->get_path();
    }

    return "";
}

void pe_displays_container::load(const std::string_view path) {
    m_pe.emplace(path);
}

void pe_displays_container::display() const {
    if (m_pe) {
        m_pe->display();
    }
}

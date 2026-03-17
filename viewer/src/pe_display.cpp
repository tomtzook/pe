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
    const auto last_dot_pos = path.find_last_of('.');
    if (last_dot_pos == std::string::npos || last_dot_pos == 0) {
        return path;
    }

    return path.substr(0, last_dot_pos);
}

static const char* frame_register_str(uint32_t frame_register) {
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
            ImGui::Text("%u", get_value_uint());
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
    : m_name("")
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
    if (ImGui::BeginTable("Info", 3, ImGuiTableFlags_Borders)) {
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
    tree_node root("root");

    {
        const auto characteristics = m_image.headers().characteristics();
        tree_node node("characteristics");
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
        tree_node node("dll-characteristics");
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

    {
        tree_node sections_root("sections");
        for (const auto& section : m_image.sections()) {
            tree_node section_node(section.name());
            section_node.add_leaf_uint("VirtualAddress", section.virtual_address());
            section_node.add_leaf_uint("VirtualSize", section.virtual_size());
            section_node.add_leaf_uint("PointerToRawData", section.pointer_to_raw_data());
            section_node.add_leaf_uint("SizeOfRawData", section.size_of_raw_data());

            {
                const auto characteristics = section.characteristics();
                tree_node node("characteristics");
                node.add_leaf_bool("typeNoPad", characteristics.bits.typeNoPad);
                node.add_leaf_bool("cntCode", characteristics.bits.cntCode);
                node.add_leaf_bool("cntInitializedData", characteristics.bits.cntInitializedData);
                node.add_leaf_bool("cntUnInitializedData", characteristics.bits.cntUnInitializedData);
                node.add_leaf_bool("lnkOther", characteristics.bits.lnkOther);
                node.add_leaf_bool("lnkiNFO", characteristics.bits.lnkiNFO);
                node.add_leaf_bool("lnkRemove", characteristics.bits.lnkRemove);
                node.add_leaf_bool("lnkComdat", characteristics.bits.lnkComdat);
                node.add_leaf_bool("gprel", characteristics.bits.gprel);
                node.add_leaf_uint("alignment", characteristics.bits.alignment); //todo: string of SectionCharacteristicsAlignment
                node.add_leaf_bool("lnkNrelocOvfl", characteristics.bits.lnkNrelocOvfl);
                node.add_leaf_bool("memDiscardable", characteristics.bits.memDiscardable);
                node.add_leaf_bool("memNotCached", characteristics.bits.memNotCached);
                node.add_leaf_bool("memNotPaged", characteristics.bits.memNotPaged);
                node.add_leaf_bool("memShared", characteristics.bits.memShared);
                node.add_leaf_bool("memExecute", characteristics.bits.memExecute);
                node.add_leaf_bool("memRead", characteristics.bits.memRead);
                node.add_leaf_bool("memWrite", characteristics.bits.memWrite);

                section_node.add_child(std::move(node));
            }

            sections_root.add_child(std::move(section_node));
        }

        root.add_child(std::move(sections_root));
    }

    if (const auto export_table = m_image.load_export_table(); export_table.is_valid()) {
        const auto export_names = export_table.names();
        tree_node export_root("exports");
        for (const auto& entry : export_table) {
            tree_node node;

            node.add_leaf_uint("rva", entry.rva());
            node.add_leaf_uint("ordinal", entry.ordinal());

            if (const auto name_entry = export_names[entry.ordinal()]; name_entry.is_valid()) {
                node.set_name(name_entry.name());
                node.add_leaf_str("name", name_entry.name());
            } else {
                node.set_name(std::format("{}", entry.ordinal()));
            }
        }

        root.add_child(std::move(export_root));
    }

    if (const auto import_table = m_image.load_import_table(); import_table.is_valid()) {
        tree_node import_root("imports");
        for (const auto& module : import_table) {
            tree_node module_node(module.name());

            for (const auto& entry : module) {
                tree_node entry_node(entry.name());
                entry_node.add_leaf_uint("ordinal", entry.ordinal());

                module_node.add_child(std::move(entry_node));
            }

            import_root.add_child(std::move(module_node));
        }

        root.add_child(std::move(import_root));
    }

    if (const auto exception_table = m_image.load_exception_table(); exception_table.is_valid()) {
        tree_node exceptions_root("exceptions");

        for (const auto& entry : exception_table) {
            tree_node entry_node(std::format("{} -> {}", entry.start(), entry.end()));

            entry_node.add_leaf_uint("Start", entry.start());
            entry_node.add_leaf_uint("End", entry.end());
            entry_node.add_leaf_uint("UnwindInfoRva", entry.unwind_info_rva());

            if (const auto unwind_info = entry.find_unwind_info(m_image.sections());
                unwind_info.is_valid()) {
                // todo: handle chained!
                tree_node unwind_node("UnwindInfo");
                unwind_node.add_leaf_uint("Flags", unwind_info.flags());
                unwind_node.add_leaf_uint("PrologSize", unwind_info.prolog_size());
                unwind_node.add_leaf_str("FrameRegister", frame_register_str(unwind_info.frame_register()));
                unwind_node.add_leaf_uint("FrameRegisterOffset", unwind_info.frame_register_offset() * 16);

                entry_node.add_child(std::move(unwind_node));
            }

            exceptions_root.add_child(std::move(entry_node));
        }

        root.add_child(std::move(exceptions_root));
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

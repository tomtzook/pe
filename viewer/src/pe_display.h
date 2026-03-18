#pragma once

#include <optional>
#include <memory>
#include <vector>
#include <pe.h>


struct tree_leaf {
    enum class type {
        empty,
        boolean,
        integer,
        uinteger,
        floating,
        string
    };

    tree_leaf(std::string_view name);

    [[nodiscard]] std::string_view get_name() const;

    [[nodiscard]] bool has_value() const;
    [[nodiscard]] type get_value_type() const;
    [[nodiscard]] bool get_value_bool() const;
    [[nodiscard]] int32_t get_value_int() const;
    [[nodiscard]] uint32_t get_value_uint() const;
    [[nodiscard]] float get_value_float() const;
    [[nodiscard]] const char* get_value_str() const;

    void set_value_bool(bool value);
    void set_value_int(int32_t value);
    void set_value_uint(uint32_t value);
    void set_value_float(float value);
    void set_value_str(const char* value);

    void draw() const;

private:
    std::string m_name;
    type m_value_type;
    union {
        bool b;
        int32_t i;
        uint32_t u;
        float f;
        const char* s;
    } m_value;
};

struct tree_node {
    tree_node();
    tree_node(std::string_view name);

    [[nodiscard]] std::string_view get_name() const;
    void set_name(std::string_view name);

    [[nodiscard]] bool has_children() const;
    [[nodiscard]] size_t get_children_count() const;

    void add_child(tree_node&& child);
    void add_leaf_bool(std::string_view name, bool value);
    void add_leaf_int(std::string_view name, int32_t value);
    void add_leaf_uint(std::string_view name, uint32_t value);
    void add_leaf_float(std::string_view name, float value);
    void add_leaf_str(std::string_view name, const char* value);

    void draw() const;

private:
    std::string m_name;

    std::vector<tree_node> m_children;
    std::vector<tree_leaf> m_leafs;
};

class pe_display {
public:
    pe_display(std::string_view path);

    [[nodiscard]] std::string_view get_path() const;
    [[nodiscard]] std::string_view get_name() const;

    void display() const;

private:
    [[nodiscard]] tree_node load_info_tree() const;
    [[nodiscard]] tree_node load_sections() const;
    [[nodiscard]] tree_node load_data_directories() const;
    [[nodiscard]] tree_node load_exports() const;
    [[nodiscard]] tree_node load_imports() const;
    [[nodiscard]] tree_node load_exceptions() const;

    std::string m_path;
    std::string m_name;
    std::vector<char> m_data;
    pe::image m_image;
    tree_node m_information_root;
};

class pe_displays_container {
public:

    [[nodiscard]] bool is_loaded() const;
    [[nodiscard]] std::string_view get_loaded_path() const;
    void load(std::string_view path);

    void display() const;

private:
    std::optional<pe_display> m_pe;
};
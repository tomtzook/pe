#pragma once

#include <stdexcept>

#include "section.h"

namespace pe {

class Exception : public std::exception {
public:
    virtual const char* what() const noexcept override {
        return "PE Exception";
    }
};

class BadHeaderException : public Exception {
public:
    enum class ProblemReason {
        DOS_MAGIC,
        NT_SIGNATURE,
        OPTIONAL_HEADER_MAGIC,
        USE_OF_DEPRECATED_CHARACTERISTIC,
    };

    explicit BadHeaderException(ProblemReason reason)
        : m_reason(reason)
    {}

    ProblemReason reason() const {
        return m_reason;
    }

    virtual const char* what() const noexcept override {
        return "Bad Header";
    }

private:
    ProblemReason m_reason;
};

class RvaNotInSectionException : public Exception {
public:
    explicit RvaNotInSectionException(const Section& section, rva_t rva)
        : m_section(section)
        , m_rva(rva)
    {}

    const Section& section() const {
        return m_section;
    }

    rva_t rva() const {
        return m_rva;
    }

    virtual const char* what() const noexcept override {
        return "RVA not in section";
    }

private:
    const Section& m_section;
    rva_t m_rva;
};

class NameNotFoundException : public Exception {
public:
    explicit NameNotFoundException(const char* name)
        : m_name(name)
    {}

    const char* name() const {
        return m_name;
    }

    virtual const char* what() const noexcept override {
        return "Name not found";
    }

private:
    const char* m_name;
};

class SectionNotFoundException : public NameNotFoundException {
public:
    explicit SectionNotFoundException(const char* name)
        : NameNotFoundException(name)
    {}

    virtual const char* what() const noexcept override {
        return "Section not found";
    }
};

class ExportNameNotFoundException : public NameNotFoundException {
public:
    explicit ExportNameNotFoundException(const char* name)
        : NameNotFoundException(name)
    {}

    virtual const char* what() const noexcept override {
        return "Exported name not found";
    }
};

class ExportOrdinalNotFoundException : public Exception {
public:
    explicit ExportOrdinalNotFoundException(export_ordinal_t ordinal)
        : m_ordinal(ordinal)
    {}

    export_ordinal_t ordinal() const {
        return m_ordinal;
    }

    virtual const char* what() const noexcept override {
        return "Export ordinal not found";
    }

private:
    export_ordinal_t m_ordinal;
};

class RvaIsForwarderException : public Exception {
public:
    explicit RvaIsForwarderException(export_ordinal_t ordinal)
        : m_ordinal(ordinal)
    {}

    export_ordinal_t ordinal() const {
        return m_ordinal;
    }

    virtual const char* what() const noexcept override {
        return "RVA is forwarder";
    }

private:
    export_ordinal_t m_ordinal;
};

class DataDirectoryNotPresent : public Exception {
public:
    explicit DataDirectoryNotPresent(DataDirectoryType type)
        : m_type(type)
    {}

    DataDirectoryType type() const {
        return m_type;
    }

    virtual const char* what() const noexcept override {
        return "Data Directory not present";
    }

private:
    DataDirectoryType m_type;
};

class RvaNotInImageException : public Exception {
public:
    explicit RvaNotInImageException(rva_t rva)
        : m_rva(rva)
    {}

    rva_t rva() const {
        return m_rva;
    }

    virtual const char* what() const noexcept override {
        return "RVA not in image";
    }

private:
    rva_t m_rva;
};

class NoEntryPointException : public Exception {
public:

    virtual const char* what() const noexcept override {
        return "No entry point";
    }
};

class NoExportTableException : public Exception {
public:

    virtual const char* what() const noexcept override {
        return "No export table";
    }
};

}

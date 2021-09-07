#pragma once

#include <stdexcept>

#include "section.h"

namespace pe {

class Exception : public std::exception {
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

private:
    const char* m_name;
};

class OrdinalNotFoundException : public Exception {
public:
    explicit OrdinalNotFoundException(export_ordinal_t ordinal)
        : m_ordinal(ordinal)
    {}

    export_ordinal_t ordinal() const {
        return m_ordinal;
    }

private:
    export_ordinal_t m_ordinal;
};

class RvaIsForwarderException : public Exception {
public:
    explicit RvaIsForwarderException(export_ordinal_t unbaisedOrdinal)
        : m_unbaisedOrdinal(unbaisedOrdinal)
    {}

    export_ordinal_t unbaisedOrdinal() const {
        return m_unbaisedOrdinal;
    }

private:
    export_ordinal_t m_unbaisedOrdinal;
};

class DataDirectoryNotPresent : public Exception {
public:
    explicit DataDirectoryNotPresent(DataDirectoryType type)
        : m_type(type)
    {}

    DataDirectoryType type() const {
        return m_type;
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

private:
    rva_t m_rva;
};

class NoEntryPointException : public Exception {

};

class NoExportTableException : public Exception {

};

}

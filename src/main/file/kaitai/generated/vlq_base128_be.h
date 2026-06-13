#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class vlq_base128_be_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

/**
 * A variable-length unsigned integer using base128 encoding. 1-byte groups
 * consist of 1-bit flag of continuation and 7-bit value chunk, and are ordered
 * "most significant group first", i.e. in "big-endian" manner.
 * 
 * This particular encoding is specified and used in:
 * 
 * * Standard MIDI file format
 * * ASN.1 BER encoding
 * * RAR 5.0 file format
 * 
 * More information on this encoding is available at
 * <https://en.wikipedia.org/wiki/Variable-length_quantity>
 * 
 * This particular implementation supports serialized values to up 8 bytes long.
 */

class vlq_base128_be_t : public kaitai::kstruct {

public:
    class group_t;

private:
    bool m__dirty;

public:

    vlq_base128_be_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, vlq_base128_be_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~vlq_base128_be_t();

    /**
     * One byte group, clearly divided into 7-bit "value" chunk and 1-bit "continuation" flag.
     */

    class group_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        group_t(kaitai::kstream* p__io, vlq_base128_be_t* p__parent = nullptr, vlq_base128_be_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~group_t();

        /**
         * If true, then we have more bytes to read
         */
        bool has_next() const { return m_has_next; }
        void set_has_next(bool _v) { m__dirty = true; m_has_next = std::move(_v); }

        /**
         * The 7-bit (base128) numeric value chunk of this group
         */
        uint64_t value() const { return m_value; }
        void set_value(uint64_t _v) { m__dirty = true; m_value = std::move(_v); }
        vlq_base128_be_t* _root() const { return m__root; }
        void set__root(vlq_base128_be_t* _v) { m__dirty = true; m__root = std::move(_v); }
        vlq_base128_be_t* _parent() const { return m__parent; }
        void set__parent(vlq_base128_be_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        bool m_has_next;
        uint64_t m_value;
        vlq_base128_be_t* m__root;
        vlq_base128_be_t* m__parent;
    };

public:
    int32_t last();
    void _invalidate_last() { f_last = false; }

    /**
     * Resulting value as normal integer
     */
    uint64_t value();
    void _invalidate_value() { f_value = false; }
    std::vector<std::unique_ptr<group_t>>* groups() const { return m_groups.get(); }
    void set_groups(std::unique_ptr<std::vector<std::unique_ptr<group_t>>> _v) { m__dirty = true; f_last = false; f_value = false; m_groups = std::move(_v); }
    vlq_base128_be_t* _root() const { return m__root; }
    void set__root(vlq_base128_be_t* _v) { m__dirty = true; f_last = false; f_value = false; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; f_last = false; f_value = false; m__parent = std::move(_v); }

private:
    bool f_last;
    int32_t m_last;
    bool f_value;
    uint64_t m_value;
    std::unique_ptr<std::vector<std::unique_ptr<group_t>>> m_groups;
    vlq_base128_be_t* m__root;
    kaitai::kstruct* m__parent;
};

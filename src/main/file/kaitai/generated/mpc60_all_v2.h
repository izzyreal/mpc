#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc60_all_v2_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include "mpc60_all_body.h"

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc60_all_v2_t : public kaitai::kstruct {

public:

    mpc60_all_v2_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc60_all_v2_t* p__root = nullptr);

private:
    void _read();

public:
    void _fetch_instances();
    ~mpc60_all_v2_t();
    std::string file_id() const { return m_file_id; }
    std::string file_format_version() const { return m_file_format_version; }
    mpc60_all_body_t::all_file_body_t* body() const { return m_body.get(); }
    mpc60_all_v2_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }

private:
    std::string m_file_id;
    std::string m_file_format_version;
    std::unique_ptr<mpc60_all_body_t::all_file_body_t> m_body;
    mpc60_all_v2_t* m__root;
    kaitai::kstruct* m__parent;
};

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_seq_v2.h"
#include "kaitai/exceptions.h"

mpc60_seq_v2_t::mpc60_seq_v2_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_seq_v2_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    _read();
}

void mpc60_seq_v2_t::_read() {
    m_file_id = m__io->read_bytes(1);
    if (!(m_file_id == std::string("\x03", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x03", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    m_file_format_version = m__io->read_bytes(1);
    if (!(m_file_format_version == std::string("\x02", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x02", 1), m_file_format_version, m__io, std::string("/seq/1"));
    }
    m_sequence = std::unique_ptr<mpc60_seq_body_t::sequence_t>(new mpc60_seq_body_t::sequence_t(m__io));
}

void mpc60_seq_v2_t::_fetch_instances() {
    m_sequence.get()->_fetch_instances();
}

mpc60_seq_v2_t::~mpc60_seq_v2_t() {}

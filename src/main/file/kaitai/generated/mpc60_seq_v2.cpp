// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_seq_v2.h"
#include "kaitai/exceptions.h"
#include <stdexcept>

mpc60_seq_v2_t::mpc60_seq_v2_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_seq_v2_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
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
    m_sequence->_read();
    m__dirty = false;
}

void mpc60_seq_v2_t::_fetch_instances() {
    m_sequence.get()->_fetch_instances();
}

void mpc60_seq_v2_t::_write() {
    m__io->write_bytes(m_file_id);
    m__io->write_bytes(m_file_format_version);
    if (m_sequence.get() == nullptr) {
        throw std::runtime_error("/seq/2: nested object is not set");
    }
    m_sequence.get()->_set_io(m__io);
    m_sequence.get()->_write();
    _fetch_instances();
    m__dirty = false;
}

void mpc60_seq_v2_t::_check() {
    if (m_file_id.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_file_id == std::string("\x03", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x03", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    if (m_file_format_version.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/1: size mismatch");
    }
    if (!(m_file_format_version == std::string("\x02", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x02", 1), m_file_format_version, m__io, std::string("/seq/1"));
    }
    if (m_sequence.get() == nullptr) {
        throw std::runtime_error("/seq/2: nested object is not set");
    }
    m_sequence.get()->_set_io(m__io);
    m_sequence.get()->_check();
    m__dirty = false;
}

mpc60_seq_v2_t::~mpc60_seq_v2_t() {}

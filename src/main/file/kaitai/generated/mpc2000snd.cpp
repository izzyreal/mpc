// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc2000snd.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>

mpc2000snd_t::mpc2000snd_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc2000snd_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc2000snd_t::_read() {
    m_magic = m__io->read_bytes(2);
    if (!(m_magic == std::string("\x01\x04", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01\x04", 2), m_magic, m__io, std::string("/seq/0"));
    }
    m_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(17), "ASCII");
    m_level = m__io->read_u1();
    m_tune = m__io->read_s1();
    m_stereo = m__io->read_bits_int_le(1);
    m_start = m__io->read_u4le();
    m_end = m__io->read_u4le();
    m_frame_count = m__io->read_u4le();
    m_loop_frame_count = m__io->read_u4le();
    m_loop_enabled = m__io->read_bits_int_le(1);
    m_beat_count = m__io->read_u1();
    m_sample_rate = m__io->read_u2le();
    m_sample_data = std::unique_ptr<std::vector<int16_t>>(new std::vector<int16_t>());
    const int l_sample_data = ((stereo()) ? (frame_count() * 2) : (frame_count()));
    for (int i = 0; i < l_sample_data; i++) {
        m_sample_data->push_back(std::move(m__io->read_s2le()));
    }
    m__dirty = false;
}

void mpc2000snd_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_sample_data->size(); ++i) {
    }
}

void mpc2000snd_t::_write() {
    m__io->write_bytes(m_magic);
    m__io->write_bytes(m_name);
    m__io->write_u1(m_level);
    m__io->write_s1(m_tune);
    m__io->write_bits_int_le(1, ((m_stereo) ? 1 : 0));
    m__io->write_u4le(m_start);
    m__io->write_u4le(m_end);
    m__io->write_u4le(m_frame_count);
    m__io->write_u4le(m_loop_frame_count);
    m__io->write_bits_int_le(1, ((m_loop_enabled) ? 1 : 0));
    m__io->write_u1(m_beat_count);
    m__io->write_u2le(m_sample_rate);
    if (m_sample_data == nullptr) {
        throw std::runtime_error("/seq/12: repeated field is not set");
    }
    for (std::vector<int16_t>::const_iterator it = m_sample_data->begin(); it != m_sample_data->end(); ++it) {
        m__io->write_s2le((*it));
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000snd_t::_check() {
    if (m_magic.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_magic == std::string("\x01\x04", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01\x04", 2), m_magic, m__io, std::string("/seq/0"));
    }
    if (m_name.size() != static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/seq/1: size mismatch");
    }
    if (m_sample_data == nullptr) {
        throw std::runtime_error("/seq/12: repeated field is not set");
    }
    if (m_sample_data->size() != static_cast<std::size_t>(((stereo()) ? (frame_count() * 2) : (frame_count())))) {
        throw std::runtime_error("/seq/12: repeat-expr size mismatch");
    }
    for (std::vector<int16_t>::const_iterator it = m_sample_data->begin(); it != m_sample_data->end(); ++it) {
    }
    m__dirty = false;
}

mpc2000snd_t::~mpc2000snd_t() {}

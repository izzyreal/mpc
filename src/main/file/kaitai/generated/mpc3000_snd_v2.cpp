// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc3000_snd_v2.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>

mpc3000_snd_v2_t::mpc3000_snd_v2_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc3000_snd_v2_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
    f_header_size = false;
    f_sample_rate = false;
}

void mpc3000_snd_v2_t::_read() {
    m_file_id = m__io->read_bytes(1);
    if (!(m_file_id == std::string("\x01", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    m_file_version = m__io->read_bytes(1);
    if (!(m_file_version == std::string("\x02", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x02", 1), m_file_version, m__io, std::string("/seq/1"));
    }
    m_name = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(17), static_cast<char>(0), false), "ASCII");
    m_level = m__io->read_u1();
    m_unknown_1 = m__io->read_bytes(2);
    m_start = m__io->read_u4le();
    m_end = m__io->read_u4le();
    m_frame_count = m__io->read_u4le();
    m_unknown_2 = m__io->read_u4le();
    m_sample_data = std::unique_ptr<std::vector<int16_t>>(new std::vector<int16_t>());
    const int l_sample_data = frame_count();
    for (int i = 0; i < l_sample_data; i++) {
        m_sample_data->push_back(std::move(m__io->read_s2le()));
    }
    m__dirty = false;
}

void mpc3000_snd_v2_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_sample_data->size(); ++i) {
    }
}

void mpc3000_snd_v2_t::_write() {
    m__io->write_bytes(m_file_id);
    m__io->write_bytes(m_file_version);
    std::string _bufm_name = m_name;
    if (_bufm_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_name += std::string(1, static_cast<char>(0));
    }
    if (_bufm_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_name.append(static_cast<std::string::size_type>(17) - _bufm_name.size(), static_cast<char>(0));
    }
    m__io->write_bytes(_bufm_name);
    m__io->write_u1(m_level);
    m__io->write_bytes(m_unknown_1);
    m__io->write_u4le(m_start);
    m__io->write_u4le(m_end);
    m__io->write_u4le(m_frame_count);
    m__io->write_u4le(m_unknown_2);
    if (m_sample_data == nullptr) {
        throw std::runtime_error("/seq/9: repeated field is not set");
    }
    for (std::vector<int16_t>::const_iterator it = m_sample_data->begin(); it != m_sample_data->end(); ++it) {
        m__io->write_s2le((*it));
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_snd_v2_t::_check() {
    if (m_file_id.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_file_id == std::string("\x01", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    if (m_file_version.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/1: size mismatch");
    }
    if (!(m_file_version == std::string("\x02", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x02", 1), m_file_version, m__io, std::string("/seq/1"));
    }
    if (m_name.size() > static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/seq/2: size mismatch");
    }
    if (m_name.find(static_cast<char>(0)) != std::string::npos) {
        throw std::runtime_error("/seq/2: terminator must not appear in value");
    }
    if (m_unknown_1.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/seq/4: size mismatch");
    }
    if (m_sample_data == nullptr) {
        throw std::runtime_error("/seq/9: repeated field is not set");
    }
    if (m_sample_data->size() != static_cast<std::size_t>(frame_count())) {
        throw std::runtime_error("/seq/9: repeat-expr size mismatch");
    }
    for (std::vector<int16_t>::const_iterator it = m_sample_data->begin(); it != m_sample_data->end(); ++it) {
    }
    m__dirty = false;
}

mpc3000_snd_v2_t::~mpc3000_snd_v2_t() {}

int8_t mpc3000_snd_v2_t::header_size() {
    if (f_header_size)
        return m_header_size;
    f_header_size = true;
    m_header_size = 38;
    return m_header_size;
}

int32_t mpc3000_snd_v2_t::sample_rate() {
    if (f_sample_rate)
        return m_sample_rate;
    f_sample_rate = true;
    m_sample_rate = 44100;
    return m_sample_rate;
}

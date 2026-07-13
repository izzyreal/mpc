// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_snd_v1.h"
#include "kaitai/exceptions.h"
#include <cstddef>
namespace mpc {
    namespace file {
        namespace kaitai {
            namespace generated {

                mpc60_snd_v1_t::mpc60_snd_v1_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_snd_v1_t* p__root) : kaitai::kstruct(p__io) {
                    m__parent = p__parent;
                    m__root = p__root ? p__root : this;
                    f_header_size = false;
                    f_packed_sample_data_byte_count = false;
                    f_packed_sample_pair_count = false;
                    f_sample_rate = false;
                    _read();
                }

                void mpc60_snd_v1_t::_read() {
                    m_file_id = m__io->read_bytes(1);
                    if (!(m_file_id == std::string("\x01", 1))) {
                        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01", 1), m_file_id, m__io, std::string("/seq/0"));
                    }
                    m_file_version = m__io->read_bytes(1);
                    if (!(m_file_version == std::string("\x01", 1))) {
                        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01", 1), m_file_version, m__io, std::string("/seq/1"));
                    }
                    m_name = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(17), static_cast<char>(0), false), "ASCII");
                    m_sample_count = m__io->read_u4le();
                    m_time_from_start_of_data_to_start_play_msec = m__io->read_u2le();
                    m_time_from_start_of_data_to_end_of_play_msec = m__io->read_u4le();
                    m_decay_time_msec = m__io->read_u2le();
                    m_volume_percent = m__io->read_u1();
                    m_tuning = m__io->read_s1();
                    m_reserved_zeroes = m__io->read_bytes(5);
                    m_velocity_to_volume_percent = m__io->read_u1();
                    m_sample_data_pairs = std::unique_ptr<std::vector<std::unique_ptr<sample_pair_t>>>(new std::vector<std::unique_ptr<sample_pair_t>>());
                    const int l_sample_data_pairs = packed_sample_pair_count();
                    for (int i = 0; i < l_sample_data_pairs; i++) {
                        m_sample_data_pairs->push_back(std::move(std::unique_ptr<sample_pair_t>(new sample_pair_t(m__io, this, m__root))));
                    }
                }

                void mpc60_snd_v1_t::_fetch_instances() {
                    for (std::size_t i = 0; i < m_sample_data_pairs->size(); ++i) {
                        m_sample_data_pairs->at(i).get()->_fetch_instances();
                    }
                }

                mpc60_snd_v1_t::~mpc60_snd_v1_t() {}

                mpc60_snd_v1_t::sample_pair_t::sample_pair_t(kaitai::kstream* p__io, mpc60_snd_v1_t* p__parent, mpc60_snd_v1_t* p__root) : kaitai::kstruct(p__io) {
                    m__parent = p__parent;
                    m__root = p__root;
                    f_sample0_code = false;
                    f_sample1_code = false;
                    _read();
                }

                void mpc60_snd_v1_t::sample_pair_t::_read() {
                    m_byte0 = m__io->read_u1();
                    m_byte1 = m__io->read_u1();
                    m_byte2 = m__io->read_u1();
                }

                void mpc60_snd_v1_t::sample_pair_t::_fetch_instances() {
                }

                mpc60_snd_v1_t::sample_pair_t::~sample_pair_t() {}

                int32_t mpc60_snd_v1_t::sample_pair_t::sample0_code() {
                    if (f_sample0_code)
                        return m_sample0_code;
                    f_sample0_code = true;
                    m_sample0_code = byte0() | (byte1() & 15) << 8;
                    return m_sample0_code;
                }

                int32_t mpc60_snd_v1_t::sample_pair_t::sample1_code() {
                    if (f_sample1_code)
                        return m_sample1_code;
                    f_sample1_code = true;
                    m_sample1_code = byte2() << 4 | (byte1() & 240) >> 4;
                    return m_sample1_code;
                }

                int8_t mpc60_snd_v1_t::header_size() {
                    if (f_header_size)
                        return m_header_size;
                    f_header_size = true;
                    m_header_size = 39;
                    return m_header_size;
                }

                int32_t mpc60_snd_v1_t::packed_sample_data_byte_count() {
                    if (f_packed_sample_data_byte_count)
                        return m_packed_sample_data_byte_count;
                    f_packed_sample_data_byte_count = true;
                    m_packed_sample_data_byte_count = packed_sample_pair_count() * 3;
                    return m_packed_sample_data_byte_count;
                }

                int32_t mpc60_snd_v1_t::packed_sample_pair_count() {
                    if (f_packed_sample_pair_count)
                        return m_packed_sample_pair_count;
                    f_packed_sample_pair_count = true;
                    m_packed_sample_pair_count = (sample_count() + 1) / 2;
                    return m_packed_sample_pair_count;
                }

                int32_t mpc60_snd_v1_t::sample_rate() {
                    if (f_sample_rate)
                        return m_sample_rate;
                    f_sample_rate = true;
                    m_sample_rate = 40000;
                    return m_sample_rate;
                }
            }
        }
    }
}

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_set_v1.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>
const std::set<mpc60_set_v1_t::analog_output_jack_t> mpc60_set_v1_t::_values_analog_output_jack_t{
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_FALSE,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_1,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_2,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_3,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_4,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_5,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_6,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_7,
    mpc60_set_v1_t::ANALOG_OUTPUT_JACK_JACK_8,
};
bool mpc60_set_v1_t::_is_defined_analog_output_jack_t(mpc60_set_v1_t::analog_output_jack_t v) {
    return mpc60_set_v1_t::_values_analog_output_jack_t.find(v) != mpc60_set_v1_t::_values_analog_output_jack_t.end();
}
const std::set<mpc60_set_v1_t::filter_range_t> mpc60_set_v1_t::_values_filter_range_t{
    mpc60_set_v1_t::FILTER_RANGE_LESS_THAN_OR_EQUALS_47_KHZ,
    mpc60_set_v1_t::FILTER_RANGE_GREATER_THAN_47_KHZ,
};
bool mpc60_set_v1_t::_is_defined_filter_range_t(mpc60_set_v1_t::filter_range_t v) {
    return mpc60_set_v1_t::_values_filter_range_t.find(v) != mpc60_set_v1_t::_values_filter_range_t.end();
}
const std::set<mpc60_set_v1_t::normal_or_hihat_t> mpc60_set_v1_t::_values_normal_or_hihat_t{
    mpc60_set_v1_t::NORMAL_OR_HIHAT_NORMAL,
    mpc60_set_v1_t::NORMAL_OR_HIHAT_HIHAT,
};
bool mpc60_set_v1_t::_is_defined_normal_or_hihat_t(mpc60_set_v1_t::normal_or_hihat_t v) {
    return mpc60_set_v1_t::_values_normal_or_hihat_t.find(v) != mpc60_set_v1_t::_values_normal_or_hihat_t.end();
}
const std::set<mpc60_set_v1_t::physical_drum_t> mpc60_set_v1_t::_values_physical_drum_t{
    mpc60_set_v1_t::PHYSICAL_DRUM_HIHT_CLSD,
    mpc60_set_v1_t::PHYSICAL_DRUM_HIHT_MEDM,
    mpc60_set_v1_t::PHYSICAL_DRUM_HIHT_OPEN,
    mpc60_set_v1_t::PHYSICAL_DRUM_SNR1,
    mpc60_set_v1_t::PHYSICAL_DRUM_SNR2,
    mpc60_set_v1_t::PHYSICAL_DRUM_BASS,
    mpc60_set_v1_t::PHYSICAL_DRUM_TOM1,
    mpc60_set_v1_t::PHYSICAL_DRUM_TOM2,
    mpc60_set_v1_t::PHYSICAL_DRUM_TOM3,
    mpc60_set_v1_t::PHYSICAL_DRUM_TOM4,
    mpc60_set_v1_t::PHYSICAL_DRUM_RID1,
    mpc60_set_v1_t::PHYSICAL_DRUM_RID2,
    mpc60_set_v1_t::PHYSICAL_DRUM_CRS1,
    mpc60_set_v1_t::PHYSICAL_DRUM_CRS2,
    mpc60_set_v1_t::PHYSICAL_DRUM_PRC1,
    mpc60_set_v1_t::PHYSICAL_DRUM_PRC2,
    mpc60_set_v1_t::PHYSICAL_DRUM_PRC3,
    mpc60_set_v1_t::PHYSICAL_DRUM_PRC4,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR01,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR02,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR03,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR04,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR05,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR06,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR07,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR08,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR09,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR10,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR11,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR12,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR13,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR14,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR15,
    mpc60_set_v1_t::PHYSICAL_DRUM_DR16,
};
bool mpc60_set_v1_t::_is_defined_physical_drum_t(mpc60_set_v1_t::physical_drum_t v) {
    return mpc60_set_v1_t::_values_physical_drum_t.find(v) != mpc60_set_v1_t::_values_physical_drum_t.end();
}
const std::set<mpc60_set_v1_t::use_master_mix_data_t> mpc60_set_v1_t::_values_use_master_mix_data_t{
    mpc60_set_v1_t::USE_MASTER_MIX_DATA_IGNORE,
    mpc60_set_v1_t::USE_MASTER_MIX_DATA_USE,
};
bool mpc60_set_v1_t::_is_defined_use_master_mix_data_t(mpc60_set_v1_t::use_master_mix_data_t v) {
    return mpc60_set_v1_t::_values_use_master_mix_data_t.find(v) != mpc60_set_v1_t::_values_use_master_mix_data_t.end();
}

mpc60_set_v1_t::mpc60_set_v1_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_set_v1_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc60_set_v1_t::_read() {
    m_file_id = m__io->read_bytes(1);
    if (!(m_file_id == std::string("\x02", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x02", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    m_file_format_version = m__io->read_bytes(1);
    if (!(m_file_format_version == std::string("\x00", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00", 1), m_file_format_version, m__io, std::string("/seq/1"));
    }
    m_total_number_of_sample_words = std::unique_ptr<u3le_t>(new u3le_t(m__io, this, m__root));
    m_total_number_of_sample_words->_read();
    m_sound_directory_entry = std::unique_ptr<std::vector<std::unique_ptr<sound_directory_entry_t>>>(new std::vector<std::unique_ptr<sound_directory_entry_t>>());
    const int l_sound_directory_entry = 34;
    for (int i = 0; i < l_sound_directory_entry; i++) {
        std::unique_ptr<sound_directory_entry_t> _t_sound_directory_entry = std::unique_ptr<sound_directory_entry_t>(new sound_directory_entry_t(m__io, this, m__root));
        try {
            _t_sound_directory_entry->_read();
        } catch(...) {
            m_sound_directory_entry->push_back(std::move(_t_sound_directory_entry));
            throw;
        }
        m_sound_directory_entry->push_back(std::move(_t_sound_directory_entry));
    }
    m_sound_map = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_sound_map = 34;
    for (int i = 0; i < l_sound_map; i++) {
        m_sound_map->push_back(std::move(m__io->read_u1()));
    }
    m_use_master_mix_data = static_cast<mpc60_set_v1_t::use_master_mix_data_t>(m__io->read_u1());
    m_master_stereo_mix = m__io->read_bytes(32);
    m_master_stereo_pan = m__io->read_bytes(32);
    m_master_echo_mix = m__io->read_bytes(32);
    m_master_drum_tuning = m__io->read_bytes(64);
    m_double_play_assignments = m__io->read_bytes(32);
    m_velocity_switch_on_off_for_each_double_play_assign = m__io->read_bytes(32);
    m_velocity_switch_threshold_value_for_each_double_play_assign = m__io->read_bytes(32);
    m__unnamed13 = m__io->read_bytes(770);
    m_sound_samples = std::unique_ptr<std::vector<uint64_t>>(new std::vector<uint64_t>());
    {
        int i = 0;
        uint64_t _;
        do {
            _ = m__io->read_bits_int_le(12);
            m_sound_samples->push_back(_);
            i++;
        } while (!(_io()->is_eof()));
    }
    m__dirty = false;
}

void mpc60_set_v1_t::_fetch_instances() {
    m_total_number_of_sample_words.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_sound_directory_entry->size(); ++i) {
        m_sound_directory_entry->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_sound_map->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_sound_samples->size(); ++i) {
    }
}

void mpc60_set_v1_t::_write() {
    m__io->write_bytes(m_file_id);
    m__io->write_bytes(m_file_format_version);
    if (m_total_number_of_sample_words.get() == nullptr) {
        throw std::runtime_error("/seq/2: nested object is not set");
    }
    m_total_number_of_sample_words.get()->_set_io(m__io);
    m_total_number_of_sample_words.get()->_write();
    if (m_sound_directory_entry == nullptr) {
        throw std::runtime_error("/seq/3: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<sound_directory_entry_t>>::const_iterator it = m_sound_directory_entry->begin(); it != m_sound_directory_entry->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_sound_map == nullptr) {
        throw std::runtime_error("/seq/4: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_sound_map->begin(); it != m_sound_map->end(); ++it) {
        m__io->write_u1((*it));
    }
    m__io->write_u1(static_cast<uint8_t>(m_use_master_mix_data));
    m__io->write_bytes(m_master_stereo_mix);
    m__io->write_bytes(m_master_stereo_pan);
    m__io->write_bytes(m_master_echo_mix);
    m__io->write_bytes(m_master_drum_tuning);
    m__io->write_bytes(m_double_play_assignments);
    m__io->write_bytes(m_velocity_switch_on_off_for_each_double_play_assign);
    m__io->write_bytes(m_velocity_switch_threshold_value_for_each_double_play_assign);
    m__io->write_bytes(m__unnamed13);
    if (m_sound_samples == nullptr) {
        throw std::runtime_error("/seq/14: repeated field is not set");
    }
    for (std::vector<uint64_t>::const_iterator it = m_sound_samples->begin(); it != m_sound_samples->end(); ++it) {
        m__io->write_bits_int_le(12, (*it));
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc60_set_v1_t::_check() {
    if (m_file_id.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_file_id == std::string("\x02", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x02", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    if (m_file_format_version.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/1: size mismatch");
    }
    if (!(m_file_format_version == std::string("\x00", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00", 1), m_file_format_version, m__io, std::string("/seq/1"));
    }
    if (m_total_number_of_sample_words.get() == nullptr) {
        throw std::runtime_error("/seq/2: nested object is not set");
    }
    m_total_number_of_sample_words.get()->_set_io(m__io);
    m_total_number_of_sample_words.get()->_check();
    if (m_sound_directory_entry == nullptr) {
        throw std::runtime_error("/seq/3: repeated field is not set");
    }
    if (m_sound_directory_entry->size() != static_cast<std::size_t>(34)) {
        throw std::runtime_error("/seq/3: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<sound_directory_entry_t>>::const_iterator it = m_sound_directory_entry->begin(); it != m_sound_directory_entry->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_sound_map == nullptr) {
        throw std::runtime_error("/seq/4: repeated field is not set");
    }
    if (m_sound_map->size() != static_cast<std::size_t>(34)) {
        throw std::runtime_error("/seq/4: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_sound_map->begin(); it != m_sound_map->end(); ++it) {
    }
    if (m_master_stereo_mix.size() != static_cast<std::string::size_type>(32)) {
        throw std::runtime_error("/seq/6: size mismatch");
    }
    if (m_master_stereo_pan.size() != static_cast<std::string::size_type>(32)) {
        throw std::runtime_error("/seq/7: size mismatch");
    }
    if (m_master_echo_mix.size() != static_cast<std::string::size_type>(32)) {
        throw std::runtime_error("/seq/8: size mismatch");
    }
    if (m_master_drum_tuning.size() != static_cast<std::string::size_type>(64)) {
        throw std::runtime_error("/seq/9: size mismatch");
    }
    if (m_double_play_assignments.size() != static_cast<std::string::size_type>(32)) {
        throw std::runtime_error("/seq/10: size mismatch");
    }
    if (m_velocity_switch_on_off_for_each_double_play_assign.size() != static_cast<std::string::size_type>(32)) {
        throw std::runtime_error("/seq/11: size mismatch");
    }
    if (m_velocity_switch_threshold_value_for_each_double_play_assign.size() != static_cast<std::string::size_type>(32)) {
        throw std::runtime_error("/seq/12: size mismatch");
    }
    if (m__unnamed13.size() != static_cast<std::string::size_type>(770)) {
        throw std::runtime_error("/seq/13: size mismatch");
    }
    if (m_sound_samples == nullptr) {
        throw std::runtime_error("/seq/14: repeated field is not set");
    }
    if (m_sound_samples->empty()) {
        throw std::runtime_error("/seq/14: repeat-until field must not be empty");
    }
    for (std::vector<uint64_t>::const_iterator it = m_sound_samples->begin(); it != m_sound_samples->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_sound_samples->begin());
        const uint64_t _ = (*it);
        const bool _is_last = (i == m_sound_samples->size() - 1);
        if ((_io()->is_eof()) != _is_last) {
            throw std::runtime_error("/seq/14: repeat-until condition mismatch");
        }
    }
    m__dirty = false;
}

mpc60_set_v1_t::~mpc60_set_v1_t() {}

mpc60_set_v1_t::sound_directory_entry_t::sound_directory_entry_t(kaitai::kstream* p__io, mpc60_set_v1_t* p__parent, mpc60_set_v1_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_set_v1_t::sound_directory_entry_t::_read() {
    m_name = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(17), static_cast<char>(0), false), "ASCII");
    m_shared_sound_link = m__io->read_u1();
    if (!(m_shared_sound_link >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_shared_sound_link, m__io, std::string("/types/sound_directory_entry/seq/1"));
    }
    if (!(m_shared_sound_link <= 33)) {
        throw kaitai::validation_greater_than_error<uint8_t>(33, m_shared_sound_link, m__io, std::string("/types/sound_directory_entry/seq/1"));
    }
    m_start_address_in_memory = m__io->read_u4le();
    m_length_in_samples = m__io->read_u4le();
    m_read_write_pointer = m__io->read_u4le();
    m_start_address_for_playing = m__io->read_u4le();
    m_time_from_start_of_data_to_start_play_msec = m__io->read_u2le();
    m_time_from_start_of_data_to_end_of_play_msec = m__io->read_u2le();
    m_attack_time_msec = m__io->read_u2le();
    m_decay_time_msec = m__io->read_u2le();
    m_pitch_factor = m__io->read_u2le();
    m_attack_rate_sent_to_sound_generator = m__io->read_u2le();
    m_decay_rate_sent_to_sound_generator = m__io->read_u2le();
    m_decay_start_time = m__io->read_u2le();
    m_sound_duration = m__io->read_u2le();
    m_analog_output_jack = static_cast<mpc60_set_v1_t::analog_output_jack_t>(m__io->read_u1());
    m_requested_stereo_mix_volume = m__io->read_u1();
    m_requested_stereo_mix_pan = m__io->read_u1();
    m_left_level_sent_to_sound_generator = m__io->read_u1();
    m_right_level_sent_to_sound_generator = m__io->read_u1();
    m_echo_level_sent_to_sound_generator = m__io->read_u1();
    m_sound_characteristics = std::unique_ptr<sound_characteristics_t>(new sound_characteristics_t(m__io, this, m__root));
    m_sound_characteristics->_read();
    m__dirty = false;
}

void mpc60_set_v1_t::sound_directory_entry_t::_fetch_instances() {
    m_sound_characteristics.get()->_fetch_instances();
}

void mpc60_set_v1_t::sound_directory_entry_t::_write() {
    std::string _bufm_name = m_name;
    if (_bufm_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_name += std::string(1, static_cast<char>(0));
    }
    if (_bufm_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_name.append(static_cast<std::string::size_type>(17) - _bufm_name.size(), static_cast<char>(0));
    }
    m__io->write_bytes(_bufm_name);
    m__io->write_u1(m_shared_sound_link);
    m__io->write_u4le(m_start_address_in_memory);
    m__io->write_u4le(m_length_in_samples);
    m__io->write_u4le(m_read_write_pointer);
    m__io->write_u4le(m_start_address_for_playing);
    m__io->write_u2le(m_time_from_start_of_data_to_start_play_msec);
    m__io->write_u2le(m_time_from_start_of_data_to_end_of_play_msec);
    m__io->write_u2le(m_attack_time_msec);
    m__io->write_u2le(m_decay_time_msec);
    m__io->write_u2le(m_pitch_factor);
    m__io->write_u2le(m_attack_rate_sent_to_sound_generator);
    m__io->write_u2le(m_decay_rate_sent_to_sound_generator);
    m__io->write_u2le(m_decay_start_time);
    m__io->write_u2le(m_sound_duration);
    m__io->write_u1(static_cast<uint8_t>(m_analog_output_jack));
    m__io->write_u1(m_requested_stereo_mix_volume);
    m__io->write_u1(m_requested_stereo_mix_pan);
    m__io->write_u1(m_left_level_sent_to_sound_generator);
    m__io->write_u1(m_right_level_sent_to_sound_generator);
    m__io->write_u1(m_echo_level_sent_to_sound_generator);
    if (m_sound_characteristics.get() == nullptr) {
        throw std::runtime_error("/types/sound_directory_entry/seq/21: nested object is not set");
    }
    m_sound_characteristics.get()->_set_io(m__io);
    m_sound_characteristics.get()->_write();
    _fetch_instances();
    m__dirty = false;
}

void mpc60_set_v1_t::sound_directory_entry_t::_check() {
    if (m_name.size() > static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/types/sound_directory_entry/seq/0: size mismatch");
    }
    if (m_name.find(static_cast<char>(0)) != std::string::npos) {
        throw std::runtime_error("/types/sound_directory_entry/seq/0: terminator must not appear in value");
    }
    if (!(m_shared_sound_link >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_shared_sound_link, m__io, std::string("/types/sound_directory_entry/seq/1"));
    }
    if (!(m_shared_sound_link <= 33)) {
        throw kaitai::validation_greater_than_error<uint8_t>(33, m_shared_sound_link, m__io, std::string("/types/sound_directory_entry/seq/1"));
    }
    if (m_sound_characteristics.get() == nullptr) {
        throw std::runtime_error("/types/sound_directory_entry/seq/21: nested object is not set");
    }
    m_sound_characteristics.get()->_set_io(m__io);
    m_sound_characteristics.get()->_check();
    m__dirty = false;
}

mpc60_set_v1_t::sound_directory_entry_t::~sound_directory_entry_t() {}

mpc60_set_v1_t::sound_directory_entry_t::sound_characteristics_t::sound_characteristics_t(kaitai::kstream* p__io, mpc60_set_v1_t::sound_directory_entry_t* p__parent, mpc60_set_v1_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_set_v1_t::sound_directory_entry_t::sound_characteristics_t::_read() {
    m_normal_or_hihat_sound = static_cast<mpc60_set_v1_t::normal_or_hihat_t>(m__io->read_bits_int_le(1));
    m_save_required = m__io->read_bits_int_le(1);
    m__unnamed2 = m__io->read_bits_int_le(5);
    m_filter = static_cast<mpc60_set_v1_t::filter_range_t>(m__io->read_bits_int_le(1));
    m__dirty = false;
}

void mpc60_set_v1_t::sound_directory_entry_t::sound_characteristics_t::_fetch_instances() {
}

void mpc60_set_v1_t::sound_directory_entry_t::sound_characteristics_t::_write() {
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_normal_or_hihat_sound));
    m__io->write_bits_int_le(1, ((m_save_required) ? 1 : 0));
    m__io->write_bits_int_le(5, m__unnamed2);
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_filter));
    _fetch_instances();
    m__dirty = false;
}

void mpc60_set_v1_t::sound_directory_entry_t::sound_characteristics_t::_check() {
    m__dirty = false;
}

mpc60_set_v1_t::sound_directory_entry_t::sound_characteristics_t::~sound_characteristics_t() {}

mpc60_set_v1_t::u3le_t::u3le_t(kaitai::kstream* p__io, mpc60_set_v1_t* p__parent, mpc60_set_v1_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_value = false;
}

void mpc60_set_v1_t::u3le_t::_read() {
    m_b12 = m__io->read_u2le();
    m_b3 = m__io->read_u1();
    m__dirty = false;
}

void mpc60_set_v1_t::u3le_t::_fetch_instances() {
}

void mpc60_set_v1_t::u3le_t::_write() {
    m__io->write_u2le(m_b12);
    m__io->write_u1(m_b3);
    _fetch_instances();
    m__dirty = false;
}

void mpc60_set_v1_t::u3le_t::_check() {
    m__dirty = false;
}

mpc60_set_v1_t::u3le_t::~u3le_t() {}

int32_t mpc60_set_v1_t::u3le_t::value() {
    if (f_value)
        return m_value;
    f_value = true;
    m_value = b12() | b3() << 16;
    return m_value;
}

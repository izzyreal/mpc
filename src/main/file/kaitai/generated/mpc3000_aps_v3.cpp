// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc3000_aps_v3.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>
const std::set<mpc3000_aps_v3_t::mix_source_t> mpc3000_aps_v3_t::_values_mix_source_t{
    mpc3000_aps_v3_t::MIX_SOURCE_MASTER,
    mpc3000_aps_v3_t::MIX_SOURCE_SEQUENCE,
    mpc3000_aps_v3_t::MIX_SOURCE_PROGRAM,
};
bool mpc3000_aps_v3_t::_is_defined_mix_source_t(mpc3000_aps_v3_t::mix_source_t v) {
    return mpc3000_aps_v3_t::_values_mix_source_t.find(v) != mpc3000_aps_v3_t::_values_mix_source_t.end();
}

mpc3000_aps_v3_t::mpc3000_aps_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc3000_aps_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc3000_aps_v3_t::_read() {
    m_file_id = m__io->read_bytes(1);
    if (!(m_file_id == std::string("\x0A", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x0A", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    m_file_version = m__io->read_bytes(1);
    if (!(m_file_version == std::string("\x00", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00", 1), m_file_version, m__io, std::string("/seq/1"));
    }
    m_num_programs = m__io->read_u1();
    m_total_samples = m__io->read_u4le();
    m_file_name = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(17), static_cast<char>(0), false), "ASCII");
    m_active_program_number = m__io->read_u1();
    m_stereo_mix_source = static_cast<mpc3000_aps_v3_t::mix_source_t>(m__io->read_u1());
    m_indiv_out_echo_send_mix_source = static_cast<mpc3000_aps_v3_t::mix_source_t>(m__io->read_u1());
    m_effects_source = static_cast<mpc3000_aps_v3_t::mix_source_t>(m__io->read_u1());
    m_record_live_mix_changes = m__io->read_u1();
    m_center_pad_16_levels_if_param_tuning = m__io->read_u1();
    m_audio_trigger_assign = m__io->read_u1();
    if (!(m_audio_trigger_assign >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_audio_trigger_assign, m__io, std::string("/seq/11"));
    }
    if (!(m_audio_trigger_assign <= 98)) {
        throw kaitai::validation_greater_than_error<uint8_t>(98, m_audio_trigger_assign, m__io, std::string("/seq/11"));
    }
    m_effects_settings = std::unique_ptr<effects_settings_t>(new effects_settings_t(m__io, this, m__root));
    m_effects_settings->_read();
    m_mixer_settings = std::unique_ptr<std::vector<std::unique_ptr<mixer_settings_t>>>(new std::vector<std::unique_ptr<mixer_settings_t>>());
    const int l_mixer_settings = 64;
    for (int i = 0; i < l_mixer_settings; i++) {
        std::unique_ptr<mixer_settings_t> _t_mixer_settings = std::unique_ptr<mixer_settings_t>(new mixer_settings_t(m__io, this, m__root));
        try {
            _t_mixer_settings->_read();
        } catch(...) {
            m_mixer_settings->push_back(std::move(_t_mixer_settings));
            throw;
        }
        m_mixer_settings->push_back(std::move(_t_mixer_settings));
    }
    m_programs = std::unique_ptr<std::vector<std::unique_ptr<program_t>>>(new std::vector<std::unique_ptr<program_t>>());
    const int l_programs = num_programs();
    for (int i = 0; i < l_programs; i++) {
        std::unique_ptr<program_t> _t_programs = std::unique_ptr<program_t>(new program_t(m__io, this, m__root));
        try {
            _t_programs->_read();
        } catch(...) {
            m_programs->push_back(std::move(_t_programs));
            throw;
        }
        m_programs->push_back(std::move(_t_programs));
    }
    m_sound_names = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    const int l_sound_names = 128;
    for (int i = 0; i < l_sound_names; i++) {
        m_sound_names->push_back(std::move(kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(17), static_cast<char>(0), false), "ASCII")));
    }
    m__dirty = false;
}

void mpc3000_aps_v3_t::_fetch_instances() {
    m_effects_settings.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_mixer_settings->size(); ++i) {
        m_mixer_settings->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_programs->size(); ++i) {
        m_programs->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_sound_names->size(); ++i) {
    }
}

void mpc3000_aps_v3_t::_write() {
    m__io->write_bytes(m_file_id);
    m__io->write_bytes(m_file_version);
    m__io->write_u1(m_num_programs);
    m__io->write_u4le(m_total_samples);
    std::string _bufm_file_name = m_file_name;
    if (_bufm_file_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_file_name += std::string(1, static_cast<char>(0));
    }
    if (_bufm_file_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_file_name.append(static_cast<std::string::size_type>(17) - _bufm_file_name.size(), static_cast<char>(0));
    }
    m__io->write_bytes(_bufm_file_name);
    m__io->write_u1(m_active_program_number);
    m__io->write_u1(static_cast<uint8_t>(m_stereo_mix_source));
    m__io->write_u1(static_cast<uint8_t>(m_indiv_out_echo_send_mix_source));
    m__io->write_u1(static_cast<uint8_t>(m_effects_source));
    m__io->write_u1(m_record_live_mix_changes);
    m__io->write_u1(m_center_pad_16_levels_if_param_tuning);
    m__io->write_u1(m_audio_trigger_assign);
    if (m_effects_settings.get() == nullptr) {
        throw std::runtime_error("/seq/12: nested object is not set");
    }
    m_effects_settings.get()->_set_io(m__io);
    m_effects_settings.get()->_write();
    if (m_mixer_settings == nullptr) {
        throw std::runtime_error("/seq/13: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mixer_settings_t>>::const_iterator it = m_mixer_settings->begin(); it != m_mixer_settings->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/13: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_programs == nullptr) {
        throw std::runtime_error("/seq/14: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<program_t>>::const_iterator it = m_programs->begin(); it != m_programs->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/14: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_sound_names == nullptr) {
        throw std::runtime_error("/seq/15: repeated field is not set");
    }
    for (std::vector<std::string>::const_iterator it = m_sound_names->begin(); it != m_sound_names->end(); ++it) {
        std::string _bufm_sound_names = (*it);
        if (_bufm_sound_names.size() < static_cast<std::string::size_type>(17)) {
            _bufm_sound_names += std::string(1, static_cast<char>(0));
        }
        if (_bufm_sound_names.size() < static_cast<std::string::size_type>(17)) {
            _bufm_sound_names.append(static_cast<std::string::size_type>(17) - _bufm_sound_names.size(), static_cast<char>(0));
        }
        m__io->write_bytes(_bufm_sound_names);
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_aps_v3_t::_check() {
    if (m_file_id.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_file_id == std::string("\x0A", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x0A", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    if (m_file_version.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/1: size mismatch");
    }
    if (!(m_file_version == std::string("\x00", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00", 1), m_file_version, m__io, std::string("/seq/1"));
    }
    if (m_file_name.size() > static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/seq/4: size mismatch");
    }
    if (m_file_name.find(static_cast<char>(0)) != std::string::npos) {
        throw std::runtime_error("/seq/4: terminator must not appear in value");
    }
    if (!(m_audio_trigger_assign >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_audio_trigger_assign, m__io, std::string("/seq/11"));
    }
    if (!(m_audio_trigger_assign <= 98)) {
        throw kaitai::validation_greater_than_error<uint8_t>(98, m_audio_trigger_assign, m__io, std::string("/seq/11"));
    }
    if (m_effects_settings.get() == nullptr) {
        throw std::runtime_error("/seq/12: nested object is not set");
    }
    m_effects_settings.get()->_set_io(m__io);
    m_effects_settings.get()->_check();
    if (m_mixer_settings == nullptr) {
        throw std::runtime_error("/seq/13: repeated field is not set");
    }
    if (m_mixer_settings->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/seq/13: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mixer_settings_t>>::const_iterator it = m_mixer_settings->begin(); it != m_mixer_settings->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/13: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_programs == nullptr) {
        throw std::runtime_error("/seq/14: repeated field is not set");
    }
    if (m_programs->size() != static_cast<std::size_t>(num_programs())) {
        throw std::runtime_error("/seq/14: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<program_t>>::const_iterator it = m_programs->begin(); it != m_programs->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/14: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_sound_names == nullptr) {
        throw std::runtime_error("/seq/15: repeated field is not set");
    }
    if (m_sound_names->size() != static_cast<std::size_t>(128)) {
        throw std::runtime_error("/seq/15: repeat-expr size mismatch");
    }
    for (std::vector<std::string>::const_iterator it = m_sound_names->begin(); it != m_sound_names->end(); ++it) {
        if ((*it).size() > static_cast<std::string::size_type>(17)) {
            throw std::runtime_error("/seq/15: size mismatch");
        }
        if ((*it).find(static_cast<char>(0)) != std::string::npos) {
            throw std::runtime_error("/seq/15: terminator must not appear in value");
        }
    }
    m__dirty = false;
}

mpc3000_aps_v3_t::~mpc3000_aps_v3_t() {}

mpc3000_aps_v3_t::effects_settings_t::effects_settings_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc3000_aps_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_aps_v3_t::effects_settings_t::_read() {
    m_effects_on = m__io->read_bits_int_be(1);
    m__unnamed1 = m__io->read_bits_int_be(7);
    m__unnamed2 = m__io->read_u1();
    m_delay_volume_tap1 = m__io->read_u1();
    m_delay_volume_tap2 = m__io->read_u1();
    m_delay_volume_tap3 = m__io->read_u1();
    m_delay_pan_tap1 = m__io->read_u1();
    m_delay_pan_tap2 = m__io->read_u1();
    m_delay_pan_tap3 = m__io->read_u1();
    m_delay_msecs_tap1 = m__io->read_u2le();
    m_delay_msecs_tap2 = m__io->read_u2le();
    m_delay_msecs_tap3 = m__io->read_u2le();
    m__unnamed12 = m__io->read_bytes(6);
    m_delay_feedback_tap1 = m__io->read_u1();
    m_delay_feedback_tap2 = m__io->read_u1();
    m_delay_feedback_tap3 = m__io->read_u1();
    m__unnamed16 = m__io->read_bytes(13);
    m__dirty = false;
}

void mpc3000_aps_v3_t::effects_settings_t::_fetch_instances() {
}

void mpc3000_aps_v3_t::effects_settings_t::_write() {
    m__io->write_bits_int_be(1, ((m_effects_on) ? 1 : 0));
    m__io->write_bits_int_be(7, m__unnamed1);
    m__io->write_u1(m__unnamed2);
    m__io->write_u1(m_delay_volume_tap1);
    m__io->write_u1(m_delay_volume_tap2);
    m__io->write_u1(m_delay_volume_tap3);
    m__io->write_u1(m_delay_pan_tap1);
    m__io->write_u1(m_delay_pan_tap2);
    m__io->write_u1(m_delay_pan_tap3);
    m__io->write_u2le(m_delay_msecs_tap1);
    m__io->write_u2le(m_delay_msecs_tap2);
    m__io->write_u2le(m_delay_msecs_tap3);
    m__io->write_bytes(m__unnamed12);
    m__io->write_u1(m_delay_feedback_tap1);
    m__io->write_u1(m_delay_feedback_tap2);
    m__io->write_u1(m_delay_feedback_tap3);
    m__io->write_bytes(m__unnamed16);
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_aps_v3_t::effects_settings_t::_check() {
    if (m__unnamed12.size() != static_cast<std::string::size_type>(6)) {
        throw std::runtime_error("/types/effects_settings/seq/12: size mismatch");
    }
    if (m__unnamed16.size() != static_cast<std::string::size_type>(13)) {
        throw std::runtime_error("/types/effects_settings/seq/16: size mismatch");
    }
    m__dirty = false;
}

mpc3000_aps_v3_t::effects_settings_t::~effects_settings_t() {}

mpc3000_aps_v3_t::mixer_settings_t::mixer_settings_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc3000_aps_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_aps_v3_t::mixer_settings_t::_read() {
    m_stereo_mix_volume = m__io->read_u1();
    m_stereo_mix_pan = m__io->read_u1();
    m_echo_volume = m__io->read_u1();
    m_out_assign_fol_ster = m__io->read_u1();
    m__dirty = false;
}

void mpc3000_aps_v3_t::mixer_settings_t::_fetch_instances() {
}

void mpc3000_aps_v3_t::mixer_settings_t::_write() {
    m__io->write_u1(m_stereo_mix_volume);
    m__io->write_u1(m_stereo_mix_pan);
    m__io->write_u1(m_echo_volume);
    m__io->write_u1(m_out_assign_fol_ster);
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_aps_v3_t::mixer_settings_t::_check() {
    m__dirty = false;
}

mpc3000_aps_v3_t::mixer_settings_t::~mixer_settings_t() {}

mpc3000_aps_v3_t::note_variation_t::note_variation_t(kaitai::kstream* p__io, mpc3000_aps_v3_t::program_t* p__parent, mpc3000_aps_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_aps_v3_t::note_variation_t::_read() {
    m_note_number_assignment = m__io->read_u1();
    m_tuning_low_range = m__io->read_u1();
    m_tuning_hi_range = m__io->read_u1();
    m_attack_low_range = m__io->read_u1();
    m_attack_hi_range = m__io->read_u1();
    m_decay_low_range = m__io->read_u1();
    m_decay_hi_range = m__io->read_u1();
    m_filter_low_range = m__io->read_u1();
    m_filter_hi_range = m__io->read_u1();
    m__dirty = false;
}

void mpc3000_aps_v3_t::note_variation_t::_fetch_instances() {
}

void mpc3000_aps_v3_t::note_variation_t::_write() {
    m__io->write_u1(m_note_number_assignment);
    m__io->write_u1(m_tuning_low_range);
    m__io->write_u1(m_tuning_hi_range);
    m__io->write_u1(m_attack_low_range);
    m__io->write_u1(m_attack_hi_range);
    m__io->write_u1(m_decay_low_range);
    m__io->write_u1(m_decay_hi_range);
    m__io->write_u1(m_filter_low_range);
    m__io->write_u1(m_filter_hi_range);
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_aps_v3_t::note_variation_t::_check() {
    m__dirty = false;
}

mpc3000_aps_v3_t::note_variation_t::~note_variation_t() {}

mpc3000_aps_v3_t::program_t::program_t(kaitai::kstream* p__io, mpc3000_aps_v3_t* p__parent, mpc3000_aps_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_aps_v3_t::program_t::_read() {
    m_program_name = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(17), static_cast<char>(0), false), "ASCII");
    m_note_variation = std::unique_ptr<note_variation_t>(new note_variation_t(m__io, this, m__root));
    m_note_variation->_read();
    m_effects_settings = std::unique_ptr<effects_settings_t>(new effects_settings_t(m__io, this, m__root));
    m_effects_settings->_read();
    m_sound_assignments = std::unique_ptr<std::vector<std::unique_ptr<sound_assignment_t>>>(new std::vector<std::unique_ptr<sound_assignment_t>>());
    const int l_sound_assignments = 64;
    for (int i = 0; i < l_sound_assignments; i++) {
        std::unique_ptr<sound_assignment_t> _t_sound_assignments = std::unique_ptr<sound_assignment_t>(new sound_assignment_t(m__io, this, m__root));
        try {
            _t_sound_assignments->_read();
        } catch(...) {
            m_sound_assignments->push_back(std::move(_t_sound_assignments));
            throw;
        }
        m_sound_assignments->push_back(std::move(_t_sound_assignments));
    }
    m_mixer_settings = std::unique_ptr<std::vector<std::unique_ptr<mixer_settings_t>>>(new std::vector<std::unique_ptr<mixer_settings_t>>());
    const int l_mixer_settings = 64;
    for (int i = 0; i < l_mixer_settings; i++) {
        std::unique_ptr<mixer_settings_t> _t_mixer_settings = std::unique_ptr<mixer_settings_t>(new mixer_settings_t(m__io, this, m__root));
        try {
            _t_mixer_settings->_read();
        } catch(...) {
            m_mixer_settings->push_back(std::move(_t_mixer_settings));
            throw;
        }
        m_mixer_settings->push_back(std::move(_t_mixer_settings));
    }
    m_pad_note_number_assignments = std::unique_ptr<std::vector<std::unique_ptr<pad_note_number_assignment_t>>>(new std::vector<std::unique_ptr<pad_note_number_assignment_t>>());
    const int l_pad_note_number_assignments = 64;
    for (int i = 0; i < l_pad_note_number_assignments; i++) {
        std::unique_ptr<pad_note_number_assignment_t> _t_pad_note_number_assignments = std::unique_ptr<pad_note_number_assignment_t>(new pad_note_number_assignment_t(m__io, this, m__root));
        try {
            _t_pad_note_number_assignments->_read();
        } catch(...) {
            m_pad_note_number_assignments->push_back(std::move(_t_pad_note_number_assignments));
            throw;
        }
        m_pad_note_number_assignments->push_back(std::move(_t_pad_note_number_assignments));
    }
    m__dirty = false;
}

void mpc3000_aps_v3_t::program_t::_fetch_instances() {
    m_note_variation.get()->_fetch_instances();
    m_effects_settings.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_sound_assignments->size(); ++i) {
        m_sound_assignments->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_mixer_settings->size(); ++i) {
        m_mixer_settings->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_pad_note_number_assignments->size(); ++i) {
        m_pad_note_number_assignments->at(i).get()->_fetch_instances();
    }
}

void mpc3000_aps_v3_t::program_t::_write() {
    std::string _bufm_program_name = m_program_name;
    if (_bufm_program_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_program_name += std::string(1, static_cast<char>(0));
    }
    if (_bufm_program_name.size() < static_cast<std::string::size_type>(17)) {
        _bufm_program_name.append(static_cast<std::string::size_type>(17) - _bufm_program_name.size(), static_cast<char>(0));
    }
    m__io->write_bytes(_bufm_program_name);
    if (m_note_variation.get() == nullptr) {
        throw std::runtime_error("/types/program/seq/1: nested object is not set");
    }
    m_note_variation.get()->_set_io(m__io);
    m_note_variation.get()->_write();
    if (m_effects_settings.get() == nullptr) {
        throw std::runtime_error("/types/program/seq/2: nested object is not set");
    }
    m_effects_settings.get()->_set_io(m__io);
    m_effects_settings.get()->_write();
    if (m_sound_assignments == nullptr) {
        throw std::runtime_error("/types/program/seq/3: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<sound_assignment_t>>::const_iterator it = m_sound_assignments->begin(); it != m_sound_assignments->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/program/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_mixer_settings == nullptr) {
        throw std::runtime_error("/types/program/seq/4: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mixer_settings_t>>::const_iterator it = m_mixer_settings->begin(); it != m_mixer_settings->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/program/seq/4: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_pad_note_number_assignments == nullptr) {
        throw std::runtime_error("/types/program/seq/5: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<pad_note_number_assignment_t>>::const_iterator it = m_pad_note_number_assignments->begin(); it != m_pad_note_number_assignments->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/program/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_aps_v3_t::program_t::_check() {
    if (m_program_name.size() > static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/types/program/seq/0: size mismatch");
    }
    if (m_program_name.find(static_cast<char>(0)) != std::string::npos) {
        throw std::runtime_error("/types/program/seq/0: terminator must not appear in value");
    }
    if (m_note_variation.get() == nullptr) {
        throw std::runtime_error("/types/program/seq/1: nested object is not set");
    }
    m_note_variation.get()->_set_io(m__io);
    m_note_variation.get()->_check();
    if (m_effects_settings.get() == nullptr) {
        throw std::runtime_error("/types/program/seq/2: nested object is not set");
    }
    m_effects_settings.get()->_set_io(m__io);
    m_effects_settings.get()->_check();
    if (m_sound_assignments == nullptr) {
        throw std::runtime_error("/types/program/seq/3: repeated field is not set");
    }
    if (m_sound_assignments->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/program/seq/3: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<sound_assignment_t>>::const_iterator it = m_sound_assignments->begin(); it != m_sound_assignments->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/program/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_mixer_settings == nullptr) {
        throw std::runtime_error("/types/program/seq/4: repeated field is not set");
    }
    if (m_mixer_settings->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/program/seq/4: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mixer_settings_t>>::const_iterator it = m_mixer_settings->begin(); it != m_mixer_settings->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/program/seq/4: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_pad_note_number_assignments == nullptr) {
        throw std::runtime_error("/types/program/seq/5: repeated field is not set");
    }
    if (m_pad_note_number_assignments->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/program/seq/5: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<pad_note_number_assignment_t>>::const_iterator it = m_pad_note_number_assignments->begin(); it != m_pad_note_number_assignments->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/program/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

mpc3000_aps_v3_t::program_t::~program_t() {}

mpc3000_aps_v3_t::program_t::pad_note_number_assignment_t::pad_note_number_assignment_t(kaitai::kstream* p__io, mpc3000_aps_v3_t::program_t* p__parent, mpc3000_aps_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_aps_v3_t::program_t::pad_note_number_assignment_t::_read() {
    m_note_number = m__io->read_u1();
    if (!(m_note_number >= 35)) {
        throw kaitai::validation_less_than_error<uint8_t>(35, m_note_number, m__io, std::string("/types/program/types/pad_note_number_assignment/seq/0"));
    }
    if (!(m_note_number <= 98)) {
        throw kaitai::validation_greater_than_error<uint8_t>(98, m_note_number, m__io, std::string("/types/program/types/pad_note_number_assignment/seq/0"));
    }
    m__dirty = false;
}

void mpc3000_aps_v3_t::program_t::pad_note_number_assignment_t::_fetch_instances() {
}

void mpc3000_aps_v3_t::program_t::pad_note_number_assignment_t::_write() {
    m__io->write_u1(m_note_number);
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_aps_v3_t::program_t::pad_note_number_assignment_t::_check() {
    if (!(m_note_number >= 35)) {
        throw kaitai::validation_less_than_error<uint8_t>(35, m_note_number, m__io, std::string("/types/program/types/pad_note_number_assignment/seq/0"));
    }
    if (!(m_note_number <= 98)) {
        throw kaitai::validation_greater_than_error<uint8_t>(98, m_note_number, m__io, std::string("/types/program/types/pad_note_number_assignment/seq/0"));
    }
    m__dirty = false;
}

mpc3000_aps_v3_t::program_t::pad_note_number_assignment_t::~pad_note_number_assignment_t() {}
const std::set<mpc3000_aps_v3_t::sound_assignment_t::decay_mode_t> mpc3000_aps_v3_t::sound_assignment_t::_values_decay_mode_t{
    mpc3000_aps_v3_t::sound_assignment_t::DECAY_MODE_END,
    mpc3000_aps_v3_t::sound_assignment_t::DECAY_MODE_START,
};
bool mpc3000_aps_v3_t::sound_assignment_t::_is_defined_decay_mode_t(mpc3000_aps_v3_t::sound_assignment_t::decay_mode_t v) {
    return mpc3000_aps_v3_t::sound_assignment_t::_values_decay_mode_t.find(v) != mpc3000_aps_v3_t::sound_assignment_t::_values_decay_mode_t.end();
}
const std::set<mpc3000_aps_v3_t::sound_assignment_t::note_variation_type_t> mpc3000_aps_v3_t::sound_assignment_t::_values_note_variation_type_t{
    mpc3000_aps_v3_t::sound_assignment_t::NOTE_VARIATION_TYPE_TUNE,
    mpc3000_aps_v3_t::sound_assignment_t::NOTE_VARIATION_TYPE_DECAY,
    mpc3000_aps_v3_t::sound_assignment_t::NOTE_VARIATION_TYPE_ATTACK,
    mpc3000_aps_v3_t::sound_assignment_t::NOTE_VARIATION_TYPE_FILTER,
};
bool mpc3000_aps_v3_t::sound_assignment_t::_is_defined_note_variation_type_t(mpc3000_aps_v3_t::sound_assignment_t::note_variation_type_t v) {
    return mpc3000_aps_v3_t::sound_assignment_t::_values_note_variation_type_t.find(v) != mpc3000_aps_v3_t::sound_assignment_t::_values_note_variation_type_t.end();
}
const std::set<mpc3000_aps_v3_t::sound_assignment_t::poly_mode_t> mpc3000_aps_v3_t::sound_assignment_t::_values_poly_mode_t{
    mpc3000_aps_v3_t::sound_assignment_t::POLY_MODE_POLY,
    mpc3000_aps_v3_t::sound_assignment_t::POLY_MODE_MONO,
    mpc3000_aps_v3_t::sound_assignment_t::POLY_MODE_NOTE_OFF,
};
bool mpc3000_aps_v3_t::sound_assignment_t::_is_defined_poly_mode_t(mpc3000_aps_v3_t::sound_assignment_t::poly_mode_t v) {
    return mpc3000_aps_v3_t::sound_assignment_t::_values_poly_mode_t.find(v) != mpc3000_aps_v3_t::sound_assignment_t::_values_poly_mode_t.end();
}
const std::set<mpc3000_aps_v3_t::sound_assignment_t::sound_generator_mode_t> mpc3000_aps_v3_t::sound_assignment_t::_values_sound_generator_mode_t{
    mpc3000_aps_v3_t::sound_assignment_t::SOUND_GENERATOR_MODE_NORMAL,
    mpc3000_aps_v3_t::sound_assignment_t::SOUND_GENERATOR_MODE_SIMULTANEOUS,
    mpc3000_aps_v3_t::sound_assignment_t::SOUND_GENERATOR_MODE_VELOCITY_SWITCH,
    mpc3000_aps_v3_t::sound_assignment_t::SOUND_GENERATOR_MODE_DECAY_SWITCH,
};
bool mpc3000_aps_v3_t::sound_assignment_t::_is_defined_sound_generator_mode_t(mpc3000_aps_v3_t::sound_assignment_t::sound_generator_mode_t v) {
    return mpc3000_aps_v3_t::sound_assignment_t::_values_sound_generator_mode_t.find(v) != mpc3000_aps_v3_t::sound_assignment_t::_values_sound_generator_mode_t.end();
}

mpc3000_aps_v3_t::sound_assignment_t::sound_assignment_t(kaitai::kstream* p__io, mpc3000_aps_v3_t::program_t* p__parent, mpc3000_aps_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_aps_v3_t::sound_assignment_t::_read() {
    m_sound_number = m__io->read_u1();
    m_sound_generator_mode = static_cast<mpc3000_aps_v3_t::sound_assignment_t::sound_generator_mode_t>(m__io->read_u1());
    m_if_over1 = m__io->read_u1();
    m_use_also_plays1 = m__io->read_u1();
    m_if_over2 = m__io->read_u1();
    m_use_also_plays2 = m__io->read_u1();
    m_poly = static_cast<mpc3000_aps_v3_t::sound_assignment_t::poly_mode_t>(m__io->read_u1());
    m_cutoff_note_1 = m__io->read_u1();
    m_cutoff_note_2 = m__io->read_u1();
    m_tune = m__io->read_s2le();
    m_attack = m__io->read_u1();
    m_decay = m__io->read_u1();
    m_decay_mode = static_cast<mpc3000_aps_v3_t::sound_assignment_t::decay_mode_t>(m__io->read_u1());
    m_filter_frequency = m__io->read_u1();
    m_filter_resonance = m__io->read_u1();
    m_filter_envel_attack = m__io->read_u1();
    m_filter_envel_decay = m__io->read_u1();
    m_filter_envel_amount = m__io->read_u1();
    m_veloc_mod_of_volume = m__io->read_u1();
    m_veloc_mod_of_attack = m__io->read_u1();
    m_veloc_mod_of_soft_start = m__io->read_u1();
    m_veloc_mod_of_filter_freq = m__io->read_u1();
    m_param = static_cast<mpc3000_aps_v3_t::sound_assignment_t::note_variation_type_t>(m__io->read_u1());
    m__dirty = false;
}

void mpc3000_aps_v3_t::sound_assignment_t::_fetch_instances() {
}

void mpc3000_aps_v3_t::sound_assignment_t::_write() {
    m__io->write_u1(m_sound_number);
    m__io->write_u1(static_cast<uint8_t>(m_sound_generator_mode));
    m__io->write_u1(m_if_over1);
    m__io->write_u1(m_use_also_plays1);
    m__io->write_u1(m_if_over2);
    m__io->write_u1(m_use_also_plays2);
    m__io->write_u1(static_cast<uint8_t>(m_poly));
    m__io->write_u1(m_cutoff_note_1);
    m__io->write_u1(m_cutoff_note_2);
    m__io->write_s2le(m_tune);
    m__io->write_u1(m_attack);
    m__io->write_u1(m_decay);
    m__io->write_u1(static_cast<uint8_t>(m_decay_mode));
    m__io->write_u1(m_filter_frequency);
    m__io->write_u1(m_filter_resonance);
    m__io->write_u1(m_filter_envel_attack);
    m__io->write_u1(m_filter_envel_decay);
    m__io->write_u1(m_filter_envel_amount);
    m__io->write_u1(m_veloc_mod_of_volume);
    m__io->write_u1(m_veloc_mod_of_attack);
    m__io->write_u1(m_veloc_mod_of_soft_start);
    m__io->write_u1(m_veloc_mod_of_filter_freq);
    m__io->write_u1(static_cast<uint8_t>(m_param));
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_aps_v3_t::sound_assignment_t::_check() {
    m__dirty = false;
}

mpc3000_aps_v3_t::sound_assignment_t::~sound_assignment_t() {}

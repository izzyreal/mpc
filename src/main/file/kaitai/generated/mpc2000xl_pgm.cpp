// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc2000xl_pgm.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>
const std::set<mpc2000xl_pgm_t::decay_mode_t> mpc2000xl_pgm_t::_values_decay_mode_t{
    mpc2000xl_pgm_t::DECAY_MODE_END,
    mpc2000xl_pgm_t::DECAY_MODE_START,
};
bool mpc2000xl_pgm_t::_is_defined_decay_mode_t(mpc2000xl_pgm_t::decay_mode_t v) {
    return mpc2000xl_pgm_t::_values_decay_mode_t.find(v) != mpc2000xl_pgm_t::_values_decay_mode_t.end();
}
const std::set<mpc2000xl_pgm_t::fx_output_t> mpc2000xl_pgm_t::_values_fx_output_t{
    mpc2000xl_pgm_t::FX_OUTPUT_NONE,
    mpc2000xl_pgm_t::FX_OUTPUT_M1,
    mpc2000xl_pgm_t::FX_OUTPUT_M2,
    mpc2000xl_pgm_t::FX_OUTPUT_R1,
    mpc2000xl_pgm_t::FX_OUTPUT_R2,
};
bool mpc2000xl_pgm_t::_is_defined_fx_output_t(mpc2000xl_pgm_t::fx_output_t v) {
    return mpc2000xl_pgm_t::_values_fx_output_t.find(v) != mpc2000xl_pgm_t::_values_fx_output_t.end();
}
const std::set<mpc2000xl_pgm_t::slider_parameter_t> mpc2000xl_pgm_t::_values_slider_parameter_t{
    mpc2000xl_pgm_t::SLIDER_PARAMETER_TUNING,
    mpc2000xl_pgm_t::SLIDER_PARAMETER_DECAY,
    mpc2000xl_pgm_t::SLIDER_PARAMETER_ATTACK,
    mpc2000xl_pgm_t::SLIDER_PARAMETER_FILTER,
};
bool mpc2000xl_pgm_t::_is_defined_slider_parameter_t(mpc2000xl_pgm_t::slider_parameter_t v) {
    return mpc2000xl_pgm_t::_values_slider_parameter_t.find(v) != mpc2000xl_pgm_t::_values_slider_parameter_t.end();
}
const std::set<mpc2000xl_pgm_t::sound_generation_mode_t> mpc2000xl_pgm_t::_values_sound_generation_mode_t{
    mpc2000xl_pgm_t::SOUND_GENERATION_MODE_NORMAL,
    mpc2000xl_pgm_t::SOUND_GENERATION_MODE_SIMULT,
    mpc2000xl_pgm_t::SOUND_GENERATION_MODE_VEL_SW,
};
bool mpc2000xl_pgm_t::_is_defined_sound_generation_mode_t(mpc2000xl_pgm_t::sound_generation_mode_t v) {
    return mpc2000xl_pgm_t::_values_sound_generation_mode_t.find(v) != mpc2000xl_pgm_t::_values_sound_generation_mode_t.end();
}
const std::set<mpc2000xl_pgm_t::voice_overlap_mode_t> mpc2000xl_pgm_t::_values_voice_overlap_mode_t{
    mpc2000xl_pgm_t::VOICE_OVERLAP_MODE_POLY,
    mpc2000xl_pgm_t::VOICE_OVERLAP_MODE_MONO,
    mpc2000xl_pgm_t::VOICE_OVERLAP_MODE_NOTE_OFF,
};
bool mpc2000xl_pgm_t::_is_defined_voice_overlap_mode_t(mpc2000xl_pgm_t::voice_overlap_mode_t v) {
    return mpc2000xl_pgm_t::_values_voice_overlap_mode_t.find(v) != mpc2000xl_pgm_t::_values_voice_overlap_mode_t.end();
}

mpc2000xl_pgm_t::mpc2000xl_pgm_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc2000xl_pgm_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc2000xl_pgm_t::_read() {
    m_magic = m__io->read_bytes(2);
    if (!(m_magic == std::string("\x07\x04", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x07\x04", 2), m_magic, m__io, std::string("/seq/0"));
    }
    m_sound_count = m__io->read_u2le();
    m_sound_names = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    const int l_sound_names = sound_count();
    for (int i = 0; i < l_sound_names; i++) {
        m_sound_names->push_back(std::move(kaitai::kstream::bytes_to_str(m__io->read_bytes(17), "ASCII")));
    }
    m__unnamed3 = m__io->read_bytes(2);
    m_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(17), "ASCII");
    m_slider = std::unique_ptr<slider_t>(new slider_t(m__io, this, m__root));
    m_slider->_read();
    m_program_change = m__io->read_u1();
    m__unnamed7 = m__io->read_bytes(5);
    m_note_parameters = std::unique_ptr<std::vector<std::unique_ptr<note_t>>>(new std::vector<std::unique_ptr<note_t>>());
    const int l_note_parameters = 64;
    for (int i = 0; i < l_note_parameters; i++) {
        std::unique_ptr<note_t> _t_note_parameters = std::unique_ptr<note_t>(new note_t(m__io, this, m__root));
        try {
            _t_note_parameters->_read();
        } catch(...) {
            m_note_parameters->push_back(std::move(_t_note_parameters));
            throw;
        }
        m_note_parameters->push_back(std::move(_t_note_parameters));
    }
    m__unnamed9 = m__io->read_bytes(1);
    m_pad_mixers = std::unique_ptr<std::vector<std::unique_ptr<pad_mixer_t>>>(new std::vector<std::unique_ptr<pad_mixer_t>>());
    const int l_pad_mixers = 64;
    for (int i = 0; i < l_pad_mixers; i++) {
        std::unique_ptr<pad_mixer_t> _t_pad_mixers = std::unique_ptr<pad_mixer_t>(new pad_mixer_t(m__io, this, m__root));
        try {
            _t_pad_mixers->_read();
        } catch(...) {
            m_pad_mixers->push_back(std::move(_t_pad_mixers));
            throw;
        }
        m_pad_mixers->push_back(std::move(_t_pad_mixers));
    }
    m__unnamed11 = m__io->read_bytes(3);
    m_pad_to_note_mapping = std::unique_ptr<std::vector<int8_t>>(new std::vector<int8_t>());
    const int l_pad_to_note_mapping = 64;
    for (int i = 0; i < l_pad_to_note_mapping; i++) {
        m_pad_to_note_mapping->push_back(std::move(m__io->read_s1()));
    }
    m__unnamed13 = m__io->read_bytes(200);
    m__dirty = false;
}

void mpc2000xl_pgm_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_sound_names->size(); ++i) {
    }
    m_slider.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_note_parameters->size(); ++i) {
        m_note_parameters->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_pad_mixers->size(); ++i) {
        m_pad_mixers->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_pad_to_note_mapping->size(); ++i) {
    }
}

void mpc2000xl_pgm_t::_write() {
    m__io->write_bytes(m_magic);
    m__io->write_u2le(m_sound_count);
    if (m_sound_names == nullptr) {
        throw std::runtime_error("/seq/2: repeated field is not set");
    }
    for (std::vector<std::string>::const_iterator it = m_sound_names->begin(); it != m_sound_names->end(); ++it) {
        m__io->write_bytes((*it));
    }
    m__io->write_bytes(m__unnamed3);
    m__io->write_bytes(m_name);
    if (m_slider.get() == nullptr) {
        throw std::runtime_error("/seq/5: nested object is not set");
    }
    m_slider.get()->_set_io(m__io);
    m_slider.get()->_write();
    m__io->write_u1(m_program_change);
    m__io->write_bytes(m__unnamed7);
    if (m_note_parameters == nullptr) {
        throw std::runtime_error("/seq/8: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<note_t>>::const_iterator it = m_note_parameters->begin(); it != m_note_parameters->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/8: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed9);
    if (m_pad_mixers == nullptr) {
        throw std::runtime_error("/seq/10: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<pad_mixer_t>>::const_iterator it = m_pad_mixers->begin(); it != m_pad_mixers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/10: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed11);
    if (m_pad_to_note_mapping == nullptr) {
        throw std::runtime_error("/seq/12: repeated field is not set");
    }
    for (std::vector<int8_t>::const_iterator it = m_pad_to_note_mapping->begin(); it != m_pad_to_note_mapping->end(); ++it) {
        m__io->write_s1((*it));
    }
    m__io->write_bytes(m__unnamed13);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_pgm_t::_check() {
    if (m_magic.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_magic == std::string("\x07\x04", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x07\x04", 2), m_magic, m__io, std::string("/seq/0"));
    }
    if (m_sound_names == nullptr) {
        throw std::runtime_error("/seq/2: repeated field is not set");
    }
    if (m_sound_names->size() != static_cast<std::size_t>(sound_count())) {
        throw std::runtime_error("/seq/2: repeat-expr size mismatch");
    }
    for (std::vector<std::string>::const_iterator it = m_sound_names->begin(); it != m_sound_names->end(); ++it) {
        if ((*it).size() != static_cast<std::string::size_type>(17)) {
            throw std::runtime_error("/seq/2: size mismatch");
        }
    }
    if (m__unnamed3.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/seq/3: size mismatch");
    }
    if (m_name.size() != static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/seq/4: size mismatch");
    }
    if (m_slider.get() == nullptr) {
        throw std::runtime_error("/seq/5: nested object is not set");
    }
    m_slider.get()->_set_io(m__io);
    m_slider.get()->_check();
    if (m__unnamed7.size() != static_cast<std::string::size_type>(5)) {
        throw std::runtime_error("/seq/7: size mismatch");
    }
    if (m_note_parameters == nullptr) {
        throw std::runtime_error("/seq/8: repeated field is not set");
    }
    if (m_note_parameters->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/seq/8: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<note_t>>::const_iterator it = m_note_parameters->begin(); it != m_note_parameters->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/8: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed9.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/9: size mismatch");
    }
    if (m_pad_mixers == nullptr) {
        throw std::runtime_error("/seq/10: repeated field is not set");
    }
    if (m_pad_mixers->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/seq/10: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<pad_mixer_t>>::const_iterator it = m_pad_mixers->begin(); it != m_pad_mixers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/10: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed11.size() != static_cast<std::string::size_type>(3)) {
        throw std::runtime_error("/seq/11: size mismatch");
    }
    if (m_pad_to_note_mapping == nullptr) {
        throw std::runtime_error("/seq/12: repeated field is not set");
    }
    if (m_pad_to_note_mapping->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/seq/12: repeat-expr size mismatch");
    }
    for (std::vector<int8_t>::const_iterator it = m_pad_to_note_mapping->begin(); it != m_pad_to_note_mapping->end(); ++it) {
    }
    if (m__unnamed13.size() != static_cast<std::string::size_type>(200)) {
        throw std::runtime_error("/seq/13: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_pgm_t::~mpc2000xl_pgm_t() {}

mpc2000xl_pgm_t::note_t::note_t(kaitai::kstream* p__io, mpc2000xl_pgm_t* p__parent, mpc2000xl_pgm_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_pgm_t::note_t::_read() {
    m_sound_index = m__io->read_u1();
    m_sound_generation_mode = static_cast<mpc2000xl_pgm_t::sound_generation_mode_t>(m__io->read_u1());
    m_velocity_range_lower = m__io->read_u1();
    m_also_play_use_note_1 = m__io->read_u1();
    m_velocity_range_upper = m__io->read_u1();
    m_also_play_use_note_2 = m__io->read_u1();
    m_voice_overlap_mode = static_cast<mpc2000xl_pgm_t::voice_overlap_mode_t>(m__io->read_u1());
    m_mute_assign_1 = m__io->read_u1();
    m_mute_assign_2 = m__io->read_u1();
    m_tune = m__io->read_s2le();
    m_attack = m__io->read_u1();
    m_decay = m__io->read_u1();
    m_decay_mode = static_cast<mpc2000xl_pgm_t::decay_mode_t>(m__io->read_u1());
    m_cutoff = m__io->read_u1();
    m_resonance = m__io->read_u1();
    m_velocity_envelope_to_filter_attack = m__io->read_u1();
    m_velocity_envelope_to_filter_decay = m__io->read_u1();
    m_velocity_envelope_to_filter_amount = m__io->read_u1();
    m_velocity_to_level = m__io->read_u1();
    m_velocity_to_attack = m__io->read_u1();
    m_velocity_to_start = m__io->read_u1();
    m_velocity_to_cutoff = m__io->read_u1();
    m_slider_parameter = static_cast<mpc2000xl_pgm_t::slider_parameter_t>(m__io->read_u1());
    m_velocity_to_pitch = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_pgm_t::note_t::_fetch_instances() {
}

void mpc2000xl_pgm_t::note_t::_write() {
    m__io->write_u1(m_sound_index);
    m__io->write_u1(static_cast<uint8_t>(m_sound_generation_mode));
    m__io->write_u1(m_velocity_range_lower);
    m__io->write_u1(m_also_play_use_note_1);
    m__io->write_u1(m_velocity_range_upper);
    m__io->write_u1(m_also_play_use_note_2);
    m__io->write_u1(static_cast<uint8_t>(m_voice_overlap_mode));
    m__io->write_u1(m_mute_assign_1);
    m__io->write_u1(m_mute_assign_2);
    m__io->write_s2le(m_tune);
    m__io->write_u1(m_attack);
    m__io->write_u1(m_decay);
    m__io->write_u1(static_cast<uint8_t>(m_decay_mode));
    m__io->write_u1(m_cutoff);
    m__io->write_u1(m_resonance);
    m__io->write_u1(m_velocity_envelope_to_filter_attack);
    m__io->write_u1(m_velocity_envelope_to_filter_decay);
    m__io->write_u1(m_velocity_envelope_to_filter_amount);
    m__io->write_u1(m_velocity_to_level);
    m__io->write_u1(m_velocity_to_attack);
    m__io->write_u1(m_velocity_to_start);
    m__io->write_u1(m_velocity_to_cutoff);
    m__io->write_u1(static_cast<uint8_t>(m_slider_parameter));
    m__io->write_u1(m_velocity_to_pitch);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_pgm_t::note_t::_check() {
    m__dirty = false;
}

mpc2000xl_pgm_t::note_t::~note_t() {}

mpc2000xl_pgm_t::pad_mixer_t::pad_mixer_t(kaitai::kstream* p__io, mpc2000xl_pgm_t* p__parent, mpc2000xl_pgm_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_pgm_t::pad_mixer_t::_read() {
    m_fx_output = static_cast<mpc2000xl_pgm_t::fx_output_t>(m__io->read_u1());
    m_volume = m__io->read_u1();
    m_pan = m__io->read_u1();
    m_volume_individual = m__io->read_u1();
    m_output = m__io->read_u1();
    m_effects_send_level = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_pgm_t::pad_mixer_t::_fetch_instances() {
}

void mpc2000xl_pgm_t::pad_mixer_t::_write() {
    m__io->write_u1(static_cast<uint8_t>(m_fx_output));
    m__io->write_u1(m_volume);
    m__io->write_u1(m_pan);
    m__io->write_u1(m_volume_individual);
    m__io->write_u1(m_output);
    m__io->write_u1(m_effects_send_level);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_pgm_t::pad_mixer_t::_check() {
    m__dirty = false;
}

mpc2000xl_pgm_t::pad_mixer_t::~pad_mixer_t() {}

mpc2000xl_pgm_t::slider_t::slider_t(kaitai::kstream* p__io, mpc2000xl_pgm_t* p__parent, mpc2000xl_pgm_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_pgm_t::slider_t::_read() {
    m_note = m__io->read_u1();
    m_tune_low = m__io->read_s1();
    m_tune_high = m__io->read_s1();
    m_decay_low = m__io->read_s1();
    m_decay_high = m__io->read_s1();
    m_attack_low = m__io->read_s1();
    m_attack_high = m__io->read_s1();
    m_filter_low = m__io->read_s1();
    m_filter_high = m__io->read_s1();
    m__dirty = false;
}

void mpc2000xl_pgm_t::slider_t::_fetch_instances() {
}

void mpc2000xl_pgm_t::slider_t::_write() {
    m__io->write_u1(m_note);
    m__io->write_s1(m_tune_low);
    m__io->write_s1(m_tune_high);
    m__io->write_s1(m_decay_low);
    m__io->write_s1(m_decay_high);
    m__io->write_s1(m_attack_low);
    m__io->write_s1(m_attack_high);
    m__io->write_s1(m_filter_low);
    m__io->write_s1(m_filter_high);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_pgm_t::slider_t::_check() {
    m__dirty = false;
}

mpc2000xl_pgm_t::slider_t::~slider_t() {}

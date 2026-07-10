// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc3000_seq_v3.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>
const std::set<mpc3000_seq_v3_t::controller_t> mpc3000_seq_v3_t::_values_controller_t{
    mpc3000_seq_v3_t::CONTROLLER_BANK_SEL_MSB,
    mpc3000_seq_v3_t::CONTROLLER_MOD_WHEEL,
    mpc3000_seq_v3_t::CONTROLLER_BREATH_CONT,
    mpc3000_seq_v3_t::CONTROLLER_CC3,
    mpc3000_seq_v3_t::CONTROLLER_FOOT_CONTROL,
    mpc3000_seq_v3_t::CONTROLLER_PORTA_TIME,
    mpc3000_seq_v3_t::CONTROLLER_DATA_ENTRY,
    mpc3000_seq_v3_t::CONTROLLER_MAIN_VOLUME,
    mpc3000_seq_v3_t::CONTROLLER_BALANCE,
    mpc3000_seq_v3_t::CONTROLLER_CC9,
    mpc3000_seq_v3_t::CONTROLLER_PAN,
    mpc3000_seq_v3_t::CONTROLLER_EXPRESSION,
    mpc3000_seq_v3_t::CONTROLLER_EFFECT_1,
    mpc3000_seq_v3_t::CONTROLLER_EFFECT_2,
    mpc3000_seq_v3_t::CONTROLLER_CC14,
    mpc3000_seq_v3_t::CONTROLLER_CC15,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_1,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_2,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_3,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_4,
    mpc3000_seq_v3_t::CONTROLLER_CC20,
    mpc3000_seq_v3_t::CONTROLLER_CC21,
    mpc3000_seq_v3_t::CONTROLLER_CC22,
    mpc3000_seq_v3_t::CONTROLLER_CC23,
    mpc3000_seq_v3_t::CONTROLLER_CC24,
    mpc3000_seq_v3_t::CONTROLLER_CC25,
    mpc3000_seq_v3_t::CONTROLLER_CC26,
    mpc3000_seq_v3_t::CONTROLLER_CC27,
    mpc3000_seq_v3_t::CONTROLLER_CC28,
    mpc3000_seq_v3_t::CONTROLLER_CC29,
    mpc3000_seq_v3_t::CONTROLLER_CC30,
    mpc3000_seq_v3_t::CONTROLLER_CC31,
    mpc3000_seq_v3_t::CONTROLLER_BANK_SEL_LSB,
    mpc3000_seq_v3_t::CONTROLLER_MOD_WHEL_LSB,
    mpc3000_seq_v3_t::CONTROLLER_BREATH_LSB,
    mpc3000_seq_v3_t::CONTROLLER_CC35,
    mpc3000_seq_v3_t::CONTROLLER_FOOT_CNT_LSB,
    mpc3000_seq_v3_t::CONTROLLER_PORT_TIME_LS,
    mpc3000_seq_v3_t::CONTROLLER_DATA_ENT_LSB,
    mpc3000_seq_v3_t::CONTROLLER_MAIN_VOL_LSB,
    mpc3000_seq_v3_t::CONTROLLER_BALANCE_LSB,
    mpc3000_seq_v3_t::CONTROLLER_CC41,
    mpc3000_seq_v3_t::CONTROLLER_PAN_LSB,
    mpc3000_seq_v3_t::CONTROLLER_EXPRESS_LSB,
    mpc3000_seq_v3_t::CONTROLLER_EFFECT_1_LSB,
    mpc3000_seq_v3_t::CONTROLLER_EFFECT_2_MSB,
    mpc3000_seq_v3_t::CONTROLLER_CC46,
    mpc3000_seq_v3_t::CONTROLLER_CC47,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_1_LS,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_2_LS,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_3_LS,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_4_LS,
    mpc3000_seq_v3_t::CONTROLLER_CC52,
    mpc3000_seq_v3_t::CONTROLLER_CC53,
    mpc3000_seq_v3_t::CONTROLLER_CC54,
    mpc3000_seq_v3_t::CONTROLLER_CC55,
    mpc3000_seq_v3_t::CONTROLLER_CC56,
    mpc3000_seq_v3_t::CONTROLLER_CC57,
    mpc3000_seq_v3_t::CONTROLLER_CC58,
    mpc3000_seq_v3_t::CONTROLLER_CC59,
    mpc3000_seq_v3_t::CONTROLLER_CC60,
    mpc3000_seq_v3_t::CONTROLLER_CC61,
    mpc3000_seq_v3_t::CONTROLLER_CC62,
    mpc3000_seq_v3_t::CONTROLLER_CC63,
    mpc3000_seq_v3_t::CONTROLLER_SUSTAIN_PDL,
    mpc3000_seq_v3_t::CONTROLLER_PORTA_PEDAL,
    mpc3000_seq_v3_t::CONTROLLER_SOSTENUTO,
    mpc3000_seq_v3_t::CONTROLLER_SOFT_PEDAL,
    mpc3000_seq_v3_t::CONTROLLER_LEGATO_FT_SW,
    mpc3000_seq_v3_t::CONTROLLER_HOLD_2,
    mpc3000_seq_v3_t::CONTROLLER_SOUND_VARI,
    mpc3000_seq_v3_t::CONTROLLER_TIMBER_HARMO,
    mpc3000_seq_v3_t::CONTROLLER_RELEASE_TIME,
    mpc3000_seq_v3_t::CONTROLLER_ATTACK_TIME,
    mpc3000_seq_v3_t::CONTROLLER_BRIGHTNESS,
    mpc3000_seq_v3_t::CONTROLLER_SOUND_CONT_6,
    mpc3000_seq_v3_t::CONTROLLER_SOUND_CONT_7,
    mpc3000_seq_v3_t::CONTROLLER_SOUND_CONT_8,
    mpc3000_seq_v3_t::CONTROLLER_SOUND_CONT_9,
    mpc3000_seq_v3_t::CONTROLLER_SOUND_CONT10,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_5,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_6,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_7,
    mpc3000_seq_v3_t::CONTROLLER_GEN_PUR_8,
    mpc3000_seq_v3_t::CONTROLLER_PORTA_CNTRL,
    mpc3000_seq_v3_t::CONTROLLER_CC85,
    mpc3000_seq_v3_t::CONTROLLER_CC86,
    mpc3000_seq_v3_t::CONTROLLER_CC87,
    mpc3000_seq_v3_t::CONTROLLER_CC88,
    mpc3000_seq_v3_t::CONTROLLER_CC89,
    mpc3000_seq_v3_t::CONTROLLER_CC90,
    mpc3000_seq_v3_t::CONTROLLER_EXT_EFF_DPTH,
    mpc3000_seq_v3_t::CONTROLLER_TREMOLO_DPTH,
    mpc3000_seq_v3_t::CONTROLLER_CHORUS_DEPTH,
    mpc3000_seq_v3_t::CONTROLLER_DETUNE_DEPTH,
    mpc3000_seq_v3_t::CONTROLLER_PHASER_DEPTH,
    mpc3000_seq_v3_t::CONTROLLER_DATA_INCRE,
    mpc3000_seq_v3_t::CONTROLLER_DATA_DECRE,
    mpc3000_seq_v3_t::CONTROLLER_NRPN_LSB,
    mpc3000_seq_v3_t::CONTROLLER_NRPN_MSB,
    mpc3000_seq_v3_t::CONTROLLER_RPN_LSB,
    mpc3000_seq_v3_t::CONTROLLER_RPN_MSB,
    mpc3000_seq_v3_t::CONTROLLER_CC102,
    mpc3000_seq_v3_t::CONTROLLER_CC103,
    mpc3000_seq_v3_t::CONTROLLER_CC104,
    mpc3000_seq_v3_t::CONTROLLER_CC105,
    mpc3000_seq_v3_t::CONTROLLER_CC106,
    mpc3000_seq_v3_t::CONTROLLER_CC107,
    mpc3000_seq_v3_t::CONTROLLER_CC108,
    mpc3000_seq_v3_t::CONTROLLER_CC109,
    mpc3000_seq_v3_t::CONTROLLER_CC110,
    mpc3000_seq_v3_t::CONTROLLER_CC111,
    mpc3000_seq_v3_t::CONTROLLER_CC112,
    mpc3000_seq_v3_t::CONTROLLER_CC113,
    mpc3000_seq_v3_t::CONTROLLER_CC114,
    mpc3000_seq_v3_t::CONTROLLER_CC115,
    mpc3000_seq_v3_t::CONTROLLER_CC116,
    mpc3000_seq_v3_t::CONTROLLER_CC117,
    mpc3000_seq_v3_t::CONTROLLER_CC118,
    mpc3000_seq_v3_t::CONTROLLER_CC119,
    mpc3000_seq_v3_t::CONTROLLER_ALL_SND_OFF,
    mpc3000_seq_v3_t::CONTROLLER_RESET_CONTRL,
    mpc3000_seq_v3_t::CONTROLLER_LOCAL_ON_OFF,
    mpc3000_seq_v3_t::CONTROLLER_ALL_NOTE_OFF,
    mpc3000_seq_v3_t::CONTROLLER_OMNI_OFF,
    mpc3000_seq_v3_t::CONTROLLER_OMNI_ON,
    mpc3000_seq_v3_t::CONTROLLER_MONO_MODE_ON,
    mpc3000_seq_v3_t::CONTROLLER_POLY_MODE_ON,
};
bool mpc3000_seq_v3_t::_is_defined_controller_t(mpc3000_seq_v3_t::controller_t v) {
    return mpc3000_seq_v3_t::_values_controller_t.find(v) != mpc3000_seq_v3_t::_values_controller_t.end();
}
const std::set<mpc3000_seq_v3_t::individual_out_t> mpc3000_seq_v3_t::_values_individual_out_t{
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_UNASSIGNED,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_1,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_2,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_3,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_4,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_5,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_6,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_7,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_OUT_8,
    mpc3000_seq_v3_t::INDIVIDUAL_OUT_INTERNAL_EFFECTS_GENERATOR,
};
bool mpc3000_seq_v3_t::_is_defined_individual_out_t(mpc3000_seq_v3_t::individual_out_t v) {
    return mpc3000_seq_v3_t::_values_individual_out_t.find(v) != mpc3000_seq_v3_t::_values_individual_out_t.end();
}
const std::set<mpc3000_seq_v3_t::mixer_event_param_t> mpc3000_seq_v3_t::_values_mixer_event_param_t{
    mpc3000_seq_v3_t::MIXER_EVENT_PARAM_STEREO_LEVEL,
    mpc3000_seq_v3_t::MIXER_EVENT_PARAM_STEREO_PAN,
    mpc3000_seq_v3_t::MIXER_EVENT_PARAM_FXSEND_LEVEL,
    mpc3000_seq_v3_t::MIXER_EVENT_PARAM_INDIV_LEVEL,
};
bool mpc3000_seq_v3_t::_is_defined_mixer_event_param_t(mpc3000_seq_v3_t::mixer_event_param_t v) {
    return mpc3000_seq_v3_t::_values_mixer_event_param_t.find(v) != mpc3000_seq_v3_t::_values_mixer_event_param_t.end();
}
const std::set<mpc3000_seq_v3_t::no_yes_t> mpc3000_seq_v3_t::_values_no_yes_t{
    mpc3000_seq_v3_t::NO_YES_FALSE,
    mpc3000_seq_v3_t::NO_YES_TRUE,
};
bool mpc3000_seq_v3_t::_is_defined_no_yes_t(mpc3000_seq_v3_t::no_yes_t v) {
    return mpc3000_seq_v3_t::_values_no_yes_t.find(v) != mpc3000_seq_v3_t::_values_no_yes_t.end();
}
const std::set<mpc3000_seq_v3_t::off_on_t> mpc3000_seq_v3_t::_values_off_on_t{
    mpc3000_seq_v3_t::OFF_ON_FALSE,
    mpc3000_seq_v3_t::OFF_ON_TRUE,
};
bool mpc3000_seq_v3_t::_is_defined_off_on_t(mpc3000_seq_v3_t::off_on_t v) {
    return mpc3000_seq_v3_t::_values_off_on_t.find(v) != mpc3000_seq_v3_t::_values_off_on_t.end();
}

mpc3000_seq_v3_t::mpc3000_seq_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    _read();
}

void mpc3000_seq_v3_t::_read() {
    m_file_id = m__io->read_bytes(1);
    if (!(m_file_id == std::string("\x03", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x03", 1), m_file_id, m__io, std::string("/seq/0"));
    }
    m_file_format_version = m__io->read_bytes(1);
    if (!(m_file_format_version == std::string("\x03", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x03", 1), m_file_format_version, m__io, std::string("/seq/1"));
    }
    m_sequence_header = std::unique_ptr<sequence_header_t>(new sequence_header_t(m__io, this, m__root));
    m_smpte_offset = std::unique_ptr<smpte_offset_t>(new smpte_offset_t(m__io, this, m__root));
    m_mixer = std::unique_ptr<std::vector<std::unique_ptr<mixer_t>>>(new std::vector<std::unique_ptr<mixer_t>>());
    const int l_mixer = 64;
    for (int i = 0; i < l_mixer; i++) {
        m_mixer->push_back(std::move(std::unique_ptr<mixer_t>(new mixer_t(m__io, this, m__root))));
    }
    m__unnamed5 = m__io->read_bytes(2);
    m_delays = std::unique_ptr<delays_t>(new delays_t(m__io, this, m__root));
    m__unnamed7 = m__io->read_bytes(3);
    m__unnamed8 = m__io->read_bytes(16);
    m_last_active_track = m__io->read_u1();
    m_number_of_tempo_changes = m__io->read_u1();
    m_number_of_active_track_headers = m__io->read_u1();
    m_track_headers = std::unique_ptr<std::vector<std::unique_ptr<track_header_t>>>(new std::vector<std::unique_ptr<track_header_t>>());
    const int l_track_headers = number_of_active_track_headers();
    for (int i = 0; i < l_track_headers; i++) {
        m_track_headers->push_back(std::move(std::unique_ptr<track_header_t>(new track_header_t(m__io, this, m__root))));
    }
    m_tempo_changes = std::unique_ptr<std::vector<std::unique_ptr<tempo_change_t>>>(new std::vector<std::unique_ptr<tempo_change_t>>());
    const int l_tempo_changes = number_of_tempo_changes();
    for (int i = 0; i < l_tempo_changes; i++) {
        m_tempo_changes->push_back(std::move(std::unique_ptr<tempo_change_t>(new tempo_change_t(m__io, this, m__root))));
    }
    m_events = std::unique_ptr<std::vector<std::unique_ptr<event_t>>>(new std::vector<std::unique_ptr<event_t>>());
    {
        int i = 0;
        while (!m__io->is_eof()) {
            m_events->push_back(std::move(std::unique_ptr<event_t>(new event_t(i == 0, ((i > 0) ? (events()->at(i - 1)->next_status()) : (255)), _root()->_io()->size() - _root()->_io()->pos(), m__io, this, m__root))));
            i++;
        }
    }
}

void mpc3000_seq_v3_t::_fetch_instances() {
    m_sequence_header.get()->_fetch_instances();
    m_smpte_offset.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_mixer->size(); ++i) {
        m_mixer->at(i).get()->_fetch_instances();
    }
    m_delays.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_track_headers->size(); ++i) {
        m_track_headers->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_tempo_changes->size(); ++i) {
        m_tempo_changes->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_events->size(); ++i) {
        m_events->at(i).get()->_fetch_instances();
    }
}

mpc3000_seq_v3_t::~mpc3000_seq_v3_t() {}

mpc3000_seq_v3_t::bar_event_t::bar_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_bar_number = false;
    _read();
}

void mpc3000_seq_v3_t::bar_event_t::_read() {
    m_bar_number1 = m__io->read_u1();
    m_bar_number2 = m__io->read_u1();
    m_numerator = m__io->read_u1();
    m_denominator = m__io->read_u1();
}

void mpc3000_seq_v3_t::bar_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::bar_event_t::~bar_event_t() {}

int32_t mpc3000_seq_v3_t::bar_event_t::bar_number() {
    if (f_bar_number)
        return m_bar_number;
    f_bar_number = true;
    m_bar_number = bar_number2() << 7 | bar_number1();
    return m_bar_number;
}

mpc3000_seq_v3_t::ch_pressure_event_t::ch_pressure_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::ch_pressure_event_t::_read() {
    m_pressure = m__io->read_u1();
    if (!(m_pressure >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_pressure, m__io, std::string("/types/ch_pressure_event/seq/0"));
    }
    if (!(m_pressure <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_pressure, m__io, std::string("/types/ch_pressure_event/seq/0"));
    }
}

void mpc3000_seq_v3_t::ch_pressure_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::ch_pressure_event_t::~ch_pressure_event_t() {}

mpc3000_seq_v3_t::control_change_event_t::control_change_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::control_change_event_t::_read() {
    m_controller = static_cast<mpc3000_seq_v3_t::controller_t>(m__io->read_u1());
    m_value = m__io->read_u1();
    if (!(m_value >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_value, m__io, std::string("/types/control_change_event/seq/1"));
    }
    if (!(m_value <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_value, m__io, std::string("/types/control_change_event/seq/1"));
    }
}

void mpc3000_seq_v3_t::control_change_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::control_change_event_t::~control_change_event_t() {}

mpc3000_seq_v3_t::delays_t::delays_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::delays_t::_read() {
    m_volume1 = m__io->read_u1();
    m_volume2 = m__io->read_u1();
    m_volume3 = m__io->read_u1();
    m_pan1 = m__io->read_u1();
    m_pan2 = m__io->read_u1();
    m_pan3 = m__io->read_u1();
    m_time1 = m__io->read_u2le();
    m_time2 = m__io->read_u2le();
    m_time3 = m__io->read_u2le();
    m_feedback1 = m__io->read_u1();
    m_feedback2 = m__io->read_u1();
    m_feedback3 = m__io->read_u1();
}

void mpc3000_seq_v3_t::delays_t::_fetch_instances() {
}

mpc3000_seq_v3_t::delays_t::~delays_t() {}

mpc3000_seq_v3_t::delta_time_event_t::delta_time_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::delta_time_event_t::_read() {
    m_delta_time = m__io->read_u2le();
}

void mpc3000_seq_v3_t::delta_time_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::delta_time_event_t::~delta_time_event_t() {}

mpc3000_seq_v3_t::event_t::event_t(bool p_is_first_event, uint8_t p_preparsed_status, int32_t p_remaining_byte_count, kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_is_first_event = p_is_first_event;
    m_preparsed_status = p_preparsed_status;
    m_remaining_byte_count = p_remaining_byte_count;
    n_parsed_status = true;
    n_track_number = true;
    n_parsed_next_status = true;
    f_mixer_event = false;
    f_next_status = false;
    f_status = false;
    _read();
}

void mpc3000_seq_v3_t::event_t::_read() {
    n_parsed_status = true;
    if (is_first_event()) {
        n_parsed_status = false;
        m_parsed_status = m__io->read_u1();
    }
    n_track_number = true;
    if ( ((status() != 168) && (status() != 136) && (status() != 255)) ) {
        n_track_number = false;
        m_track_number = m__io->read_u1();
    }
    switch (status()) {
    case 136: {
        m_event_body = std::unique_ptr<delta_time_event_t>(new delta_time_event_t(m__io, this, m__root));
        break;
    }
    case 144: {
        m_event_body = std::unique_ptr<note_event_t>(new note_event_t(m__io, this, m__root));
        break;
    }
    case 152: {
        m_event_body = std::unique_ptr<note_event_t>(new note_event_t(m__io, this, m__root));
        break;
    }
    case 160: {
        m_event_body = std::unique_ptr<poly_pressure_event_t>(new poly_pressure_event_t(m__io, this, m__root));
        break;
    }
    case 168: {
        m_event_body = std::unique_ptr<bar_event_t>(new bar_event_t(m__io, this, m__root));
        break;
    }
    case 176: {
        m_event_body = std::unique_ptr<control_change_event_t>(new control_change_event_t(m__io, this, m__root));
        break;
    }
    case 192: {
        m_event_body = std::unique_ptr<program_change_event_t>(new program_change_event_t(m__io, this, m__root));
        break;
    }
    case 208: {
        m_event_body = std::unique_ptr<ch_pressure_event_t>(new ch_pressure_event_t(m__io, this, m__root));
        break;
    }
    case 224: {
        m_event_body = std::unique_ptr<pitch_bend_event_t>(new pitch_bend_event_t(m__io, this, m__root));
        break;
    }
    case 232: {
        m_event_body = std::unique_ptr<tune_request_event_t>(new tune_request_event_t(m__io, this, m__root));
        break;
    }
    }
    if (status() == 240) {
        m_system_exclusive_body = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
        {
            int i = 0;
            uint8_t _;
            do {
                _ = m__io->read_u1();
                m_system_exclusive_body->push_back(_);
                i++;
            } while (!( ((i >= remaining_byte_count()) || (_ > 127)) ));
        }
    }
    n_parsed_next_status = true;
    if ( ((!(status() == 240)) && (remaining_byte_count() > 2)) ) {
        n_parsed_next_status = false;
        m_parsed_next_status = m__io->read_u1();
    }
}

void mpc3000_seq_v3_t::event_t::_fetch_instances() {
    n_parsed_status = true;
    if (is_first_event()) {
        n_parsed_status = false;
    }
    n_track_number = true;
    if ( ((status() != 168) && (status() != 136) && (status() != 255)) ) {
        n_track_number = false;
    }
    switch (status()) {
    case 136: {
        {
            delta_time_event_t* _switch_obj = dynamic_cast<delta_time_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 144: {
        {
            note_event_t* _switch_obj = dynamic_cast<note_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 152: {
        {
            note_event_t* _switch_obj = dynamic_cast<note_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 160: {
        {
            poly_pressure_event_t* _switch_obj = dynamic_cast<poly_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 168: {
        {
            bar_event_t* _switch_obj = dynamic_cast<bar_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 176: {
        {
            control_change_event_t* _switch_obj = dynamic_cast<control_change_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 192: {
        {
            program_change_event_t* _switch_obj = dynamic_cast<program_change_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 208: {
        {
            ch_pressure_event_t* _switch_obj = dynamic_cast<ch_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 224: {
        {
            pitch_bend_event_t* _switch_obj = dynamic_cast<pitch_bend_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 232: {
        {
            tune_request_event_t* _switch_obj = dynamic_cast<tune_request_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    }
    if (status() == 240) {
        for (std::size_t i = 0; i < m_system_exclusive_body->size(); ++i) {
        }
    }
    n_parsed_next_status = true;
    if ( ((!(status() == 240)) && (remaining_byte_count() > 2)) ) {
        n_parsed_next_status = false;
    }
    mixer_event();
    if (f_mixer_event) {
        m_mixer_event.get()->_fetch_instances();
    }
}

mpc3000_seq_v3_t::event_t::~event_t() {}

mpc3000_seq_v3_t::mixer_event_t* mpc3000_seq_v3_t::event_t::mixer_event() {
    if (f_mixer_event)
        return m_mixer_event.get();
    f_mixer_event = true;
    if ( ((status() == 240) && (system_exclusive_body()->size() >= 7) && (system_exclusive_body()->at(0) == 71) && (system_exclusive_body()->at(1) == 0) && (system_exclusive_body()->at(2) == 68) && (system_exclusive_body()->at(3) == 69)) ) {
        m_mixer_event = std::unique_ptr<mixer_event_t>(new mixer_event_t(system_exclusive_body(), m__io, this, m__root));
    }
    return m_mixer_event.get();
}

uint8_t mpc3000_seq_v3_t::event_t::next_status() {
    if (f_next_status)
        return m_next_status;
    f_next_status = true;
    m_next_status = ((status() == 240) ? (system_exclusive_body()->back()) : (parsed_next_status()));
    return m_next_status;
}

uint8_t mpc3000_seq_v3_t::event_t::status() {
    if (f_status)
        return m_status;
    f_status = true;
    m_status = ((preparsed_status() == 255) ? (parsed_status()) : (preparsed_status()));
    return m_status;
}

mpc3000_seq_v3_t::mixer_t::mixer_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::mixer_t::_read() {
    m_stereo_mix = m__io->read_u1();
    if (!(m_stereo_mix >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_stereo_mix, m__io, std::string("/types/mixer/seq/0"));
    }
    if (!(m_stereo_mix <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_stereo_mix, m__io, std::string("/types/mixer/seq/0"));
    }
    m_stereo_pan = m__io->read_u1();
    if (!(m_stereo_pan >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_stereo_pan, m__io, std::string("/types/mixer/seq/1"));
    }
    if (!(m_stereo_pan <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_stereo_pan, m__io, std::string("/types/mixer/seq/1"));
    }
    m_individual_out_mix = m__io->read_u1();
    m_individual_out = static_cast<mpc3000_seq_v3_t::individual_out_t>(m__io->read_bits_int_le(7));
    m_follow_stereo = static_cast<mpc3000_seq_v3_t::no_yes_t>(m__io->read_bits_int_le(1));
}

void mpc3000_seq_v3_t::mixer_t::_fetch_instances() {
}

mpc3000_seq_v3_t::mixer_t::~mixer_t() {}

mpc3000_seq_v3_t::mixer_event_t::mixer_event_t(std::vector<uint8_t>* p_data, kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_data = p_data;
    f_pad_index = false;
    f_param = false;
    f_value = false;
    _read();
}

void mpc3000_seq_v3_t::mixer_event_t::_read() {
}

void mpc3000_seq_v3_t::mixer_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::mixer_event_t::~mixer_event_t() {}

uint8_t mpc3000_seq_v3_t::mixer_event_t::pad_index() {
    if (f_pad_index)
        return m_pad_index;
    f_pad_index = true;
    m_pad_index = data()->at(5);
    return m_pad_index;
}

mpc3000_seq_v3_t::mixer_event_param_t mpc3000_seq_v3_t::mixer_event_t::param() {
    if (f_param)
        return m_param;
    f_param = true;
    m_param = static_cast<mpc3000_seq_v3_t::mixer_event_param_t>(data()->at(4));
    return m_param;
}

uint8_t mpc3000_seq_v3_t::mixer_event_t::value() {
    if (f_value)
        return m_value;
    f_value = true;
    m_value = data()->at(6);
    return m_value;
}
const std::set<mpc3000_seq_v3_t::note_event_t::note_variation_type_t> mpc3000_seq_v3_t::note_event_t::_values_note_variation_type_t{
    mpc3000_seq_v3_t::note_event_t::NOTE_VARIATION_TYPE_TUNE,
    mpc3000_seq_v3_t::note_event_t::NOTE_VARIATION_TYPE_DECAY,
    mpc3000_seq_v3_t::note_event_t::NOTE_VARIATION_TYPE_ATTACK,
    mpc3000_seq_v3_t::note_event_t::NOTE_VARIATION_TYPE_FILTER,
};
bool mpc3000_seq_v3_t::note_event_t::_is_defined_note_variation_type_t(mpc3000_seq_v3_t::note_event_t::note_variation_type_t v) {
    return mpc3000_seq_v3_t::note_event_t::_values_note_variation_type_t.find(v) != mpc3000_seq_v3_t::note_event_t::_values_note_variation_type_t.end();
}

mpc3000_seq_v3_t::note_event_t::note_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_duration = false;
    f_note_variation_type = false;
    _read();
}

void mpc3000_seq_v3_t::note_event_t::_read() {
    m_note_number = m__io->read_u1();
    if (!(m_note_number >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_note_number, m__io, std::string("/types/note_event/seq/0"));
    }
    if (!(m_note_number <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_note_number, m__io, std::string("/types/note_event/seq/0"));
    }
    m_velocity = m__io->read_u1();
    if (!(m_velocity >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_velocity, m__io, std::string("/types/note_event/seq/1"));
    }
    if (!(m_velocity <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_velocity, m__io, std::string("/types/note_event/seq/1"));
    }
    m_note_variation_value = m__io->read_u1();
    m_duration_byte_1 = m__io->read_u1();
    m_duration_byte_2 = m__io->read_u1();
}

void mpc3000_seq_v3_t::note_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::note_event_t::~note_event_t() {}

int32_t mpc3000_seq_v3_t::note_event_t::duration() {
    if (f_duration)
        return m_duration;
    f_duration = true;
    m_duration = duration_byte_1() + (duration_byte_2() << 7);
    return m_duration;
}

mpc3000_seq_v3_t::note_event_t::note_variation_type_t mpc3000_seq_v3_t::note_event_t::note_variation_type() {
    if (f_note_variation_type)
        return m_note_variation_type;
    f_note_variation_type = true;
    m_note_variation_type = static_cast<mpc3000_seq_v3_t::note_event_t::note_variation_type_t>(static_cast<mpc3000_seq_v3_t::event_t*>(_parent())->status() - 152);
    return m_note_variation_type;
}

mpc3000_seq_v3_t::pitch_bend_event_t::pitch_bend_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_corrected_pitch_bend_amount = false;
    _read();
}

void mpc3000_seq_v3_t::pitch_bend_event_t::_read() {
    m_pitch_bend_amount_bits_1 = m__io->read_bits_int_le(8);
    m_pitch_bend_amount_bits_2 = m__io->read_bits_int_le(8);
}

void mpc3000_seq_v3_t::pitch_bend_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::pitch_bend_event_t::~pitch_bend_event_t() {}

int32_t mpc3000_seq_v3_t::pitch_bend_event_t::corrected_pitch_bend_amount() {
    if (f_corrected_pitch_bend_amount)
        return m_corrected_pitch_bend_amount;
    f_corrected_pitch_bend_amount = true;
    m_corrected_pitch_bend_amount = (pitch_bend_amount_bits_1() + (pitch_bend_amount_bits_2() << 7)) - 8192;
    return m_corrected_pitch_bend_amount;
}

mpc3000_seq_v3_t::poly_pressure_event_t::poly_pressure_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::poly_pressure_event_t::_read() {
    m_note = m__io->read_u1();
    if (!(m_note >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_note, m__io, std::string("/types/poly_pressure_event/seq/0"));
    }
    if (!(m_note <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_note, m__io, std::string("/types/poly_pressure_event/seq/0"));
    }
    m_pressure = m__io->read_u1();
    if (!(m_pressure >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_pressure, m__io, std::string("/types/poly_pressure_event/seq/1"));
    }
    if (!(m_pressure <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_pressure, m__io, std::string("/types/poly_pressure_event/seq/1"));
    }
}

void mpc3000_seq_v3_t::poly_pressure_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::poly_pressure_event_t::~poly_pressure_event_t() {}

mpc3000_seq_v3_t::program_change_event_t::program_change_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::program_change_event_t::_read() {
    m_program = m__io->read_u1();
    if (!(m_program >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_program, m__io, std::string("/types/program_change_event/seq/0"));
    }
    if (!(m_program <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_program, m__io, std::string("/types/program_change_event/seq/0"));
    }
}

void mpc3000_seq_v3_t::program_change_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::program_change_event_t::~program_change_event_t() {}

mpc3000_seq_v3_t::sequence_header_t::sequence_header_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::sequence_header_t::_read() {
    m_sequence_number = m__io->read_u1();
    m_sequence_length_in_bytes = std::unique_ptr<u3le_t>(new u3le_t(m__io, this, m__root));
    m__unnamed2 = m__io->read_bytes(5);
    m_sequence_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m__unnamed4 = m__io->read_bytes(1);
    m_loop_to_bar = static_cast<mpc3000_seq_v3_t::off_on_t>(m__io->read_bits_int_le(1));
    m__unnamed6 = m__io->read_bits_int_le(7);
    m_loop_to_bar_number = m__io->read_u2le();
    m_number_of_bars = m__io->read_u2le();
    m_length_in_ticks = m__io->read_u4le();
    m_tempo = m__io->read_u2le();
}

void mpc3000_seq_v3_t::sequence_header_t::_fetch_instances() {
    m_sequence_length_in_bytes.get()->_fetch_instances();
}

mpc3000_seq_v3_t::sequence_header_t::~sequence_header_t() {}

mpc3000_seq_v3_t::smpte_offset_t::smpte_offset_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::smpte_offset_t::_read() {
    m_hundredth_frames = m__io->read_u1();
    if (!(m_hundredth_frames >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_hundredth_frames, m__io, std::string("/types/smpte_offset/seq/0"));
    }
    if (!(m_hundredth_frames <= 99)) {
        throw kaitai::validation_greater_than_error<uint8_t>(99, m_hundredth_frames, m__io, std::string("/types/smpte_offset/seq/0"));
    }
    m_frames = m__io->read_u1();
    if (!(m_frames >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_frames, m__io, std::string("/types/smpte_offset/seq/1"));
    }
    if (!(m_frames <= 29)) {
        throw kaitai::validation_greater_than_error<uint8_t>(29, m_frames, m__io, std::string("/types/smpte_offset/seq/1"));
    }
    m_seconds = m__io->read_u1();
    if (!(m_seconds >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_seconds, m__io, std::string("/types/smpte_offset/seq/2"));
    }
    if (!(m_seconds <= 59)) {
        throw kaitai::validation_greater_than_error<uint8_t>(59, m_seconds, m__io, std::string("/types/smpte_offset/seq/2"));
    }
    m_minutes = m__io->read_u1();
    if (!(m_minutes >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_minutes, m__io, std::string("/types/smpte_offset/seq/3"));
    }
    if (!(m_minutes <= 59)) {
        throw kaitai::validation_greater_than_error<uint8_t>(59, m_minutes, m__io, std::string("/types/smpte_offset/seq/3"));
    }
    m_hours = m__io->read_u1();
    if (!(m_hours >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_hours, m__io, std::string("/types/smpte_offset/seq/4"));
    }
    if (!(m_hours <= 23)) {
        throw kaitai::validation_greater_than_error<uint8_t>(23, m_hours, m__io, std::string("/types/smpte_offset/seq/4"));
    }
}

void mpc3000_seq_v3_t::smpte_offset_t::_fetch_instances() {
}

mpc3000_seq_v3_t::smpte_offset_t::~smpte_offset_t() {}

mpc3000_seq_v3_t::tempo_change_t::tempo_change_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_factor_percentage = false;
    _read();
}

void mpc3000_seq_v3_t::tempo_change_t::_read() {
    m_ticks_from_sequence_start = m__io->read_u4le();
    m_factor1 = m__io->read_bits_int_le(12);
    m_factor2 = m__io->read_bits_int_le(4);
}

void mpc3000_seq_v3_t::tempo_change_t::_fetch_instances() {
}

mpc3000_seq_v3_t::tempo_change_t::~tempo_change_t() {}

double mpc3000_seq_v3_t::tempo_change_t::factor_percentage() {
    if (f_factor_percentage)
        return m_factor_percentage;
    f_factor_percentage = true;
    m_factor_percentage = (factor1() / 4096.0) * 100 + factor2() * 100;
    return m_factor_percentage;
}

mpc3000_seq_v3_t::track_header_t::track_header_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    n__unnamed1 = true;
    n_user_track_number = true;
    n_track_mute = true;
    n_track_in_use = true;
    n_drum_track = true;
    n__unnamed6 = true;
    n_primary_port_channel_assignment = true;
    n_secondary_port_channel_assignment = true;
    n_track_name = true;
    n_track_volume = true;
    n_program_change_number = true;
    n__unnamed12 = true;
    _read();
}

void mpc3000_seq_v3_t::track_header_t::_read() {
    m_absolute_recorded_track_number = m__io->read_s1();
    n__unnamed1 = true;
    if (absolute_recorded_track_number() == -1) {
        n__unnamed1 = false;
        m__unnamed1 = m__io->read_bytes(23);
    }
    n_user_track_number = true;
    if (absolute_recorded_track_number() != -1) {
        n_user_track_number = false;
        m_user_track_number = m__io->read_u1();
    }
    n_track_mute = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_mute = false;
        m_track_mute = m__io->read_bits_int_le(1);
    }
    n_track_in_use = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_in_use = false;
        m_track_in_use = m__io->read_bits_int_le(1);
    }
    n_drum_track = true;
    if (absolute_recorded_track_number() != -1) {
        n_drum_track = false;
        m_drum_track = m__io->read_bits_int_le(1);
    }
    n__unnamed6 = true;
    if (absolute_recorded_track_number() != -1) {
        n__unnamed6 = false;
        m__unnamed6 = m__io->read_bits_int_le(5);
    }
    n_primary_port_channel_assignment = true;
    if (absolute_recorded_track_number() != -1) {
        n_primary_port_channel_assignment = false;
        m_primary_port_channel_assignment = m__io->read_u1();
    }
    n_secondary_port_channel_assignment = true;
    if (absolute_recorded_track_number() != -1) {
        n_secondary_port_channel_assignment = false;
        m_secondary_port_channel_assignment = m__io->read_s1();
    }
    n_track_name = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_name = false;
        m_track_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    }
    n_track_volume = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_volume = false;
        m_track_volume = m__io->read_u1();
        if (!(m_track_volume >= 1)) {
            throw kaitai::validation_less_than_error<uint8_t>(1, m_track_volume, m__io, std::string("/types/track_header/seq/10"));
        }
        if (!(m_track_volume <= 200)) {
            throw kaitai::validation_greater_than_error<uint8_t>(200, m_track_volume, m__io, std::string("/types/track_header/seq/10"));
        }
    }
    n_program_change_number = true;
    if (absolute_recorded_track_number() != -1) {
        n_program_change_number = false;
        m_program_change_number = m__io->read_u1();
    }
    n__unnamed12 = true;
    if (absolute_recorded_track_number() != -1) {
        n__unnamed12 = false;
        m__unnamed12 = m__io->read_bytes(1);
    }
}

void mpc3000_seq_v3_t::track_header_t::_fetch_instances() {
    n__unnamed1 = true;
    if (absolute_recorded_track_number() == -1) {
        n__unnamed1 = false;
    }
    n_user_track_number = true;
    if (absolute_recorded_track_number() != -1) {
        n_user_track_number = false;
    }
    n_track_mute = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_mute = false;
    }
    n_track_in_use = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_in_use = false;
    }
    n_drum_track = true;
    if (absolute_recorded_track_number() != -1) {
        n_drum_track = false;
    }
    n__unnamed6 = true;
    if (absolute_recorded_track_number() != -1) {
        n__unnamed6 = false;
    }
    n_primary_port_channel_assignment = true;
    if (absolute_recorded_track_number() != -1) {
        n_primary_port_channel_assignment = false;
    }
    n_secondary_port_channel_assignment = true;
    if (absolute_recorded_track_number() != -1) {
        n_secondary_port_channel_assignment = false;
    }
    n_track_name = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_name = false;
    }
    n_track_volume = true;
    if (absolute_recorded_track_number() != -1) {
        n_track_volume = false;
    }
    n_program_change_number = true;
    if (absolute_recorded_track_number() != -1) {
        n_program_change_number = false;
    }
    n__unnamed12 = true;
    if (absolute_recorded_track_number() != -1) {
        n__unnamed12 = false;
    }
}

mpc3000_seq_v3_t::track_header_t::~track_header_t() {}

mpc3000_seq_v3_t::tune_request_event_t::tune_request_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_seq_v3_t::tune_request_event_t::_read() {
    m_tune_request_event = m__io->read_bytes(0);
}

void mpc3000_seq_v3_t::tune_request_event_t::_fetch_instances() {
}

mpc3000_seq_v3_t::tune_request_event_t::~tune_request_event_t() {}

mpc3000_seq_v3_t::u3le_t::u3le_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::sequence_header_t* p__parent, mpc3000_seq_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_value = false;
    _read();
}

void mpc3000_seq_v3_t::u3le_t::_read() {
    m_b12 = m__io->read_u2le();
    m_b3 = m__io->read_u1();
}

void mpc3000_seq_v3_t::u3le_t::_fetch_instances() {
}

mpc3000_seq_v3_t::u3le_t::~u3le_t() {}

int32_t mpc3000_seq_v3_t::u3le_t::value() {
    if (f_value)
        return m_value;
    f_value = true;
    m_value = b12() | b3() << 16;
    return m_value;
}

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc2000xl_all.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>
const std::set<mpc2000xl_all_t::bus_t> mpc2000xl_all_t::_values_bus_t{
    mpc2000xl_all_t::BUS_MIDI,
    mpc2000xl_all_t::BUS_DRUM1,
    mpc2000xl_all_t::BUS_DRUM2,
    mpc2000xl_all_t::BUS_DRUM3,
    mpc2000xl_all_t::BUS_DRUM4,
};
bool mpc2000xl_all_t::_is_defined_bus_t(mpc2000xl_all_t::bus_t v) {
    return mpc2000xl_all_t::_values_bus_t.find(v) != mpc2000xl_all_t::_values_bus_t.end();
}
const std::set<mpc2000xl_all_t::click_output_t> mpc2000xl_all_t::_values_click_output_t{
    mpc2000xl_all_t::CLICK_OUTPUT_STEREO,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV1,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV2,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV3,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV4,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV5,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV6,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV7,
    mpc2000xl_all_t::CLICK_OUTPUT_INDIV8,
};
bool mpc2000xl_all_t::_is_defined_click_output_t(mpc2000xl_all_t::click_output_t v) {
    return mpc2000xl_all_t::_values_click_output_t.find(v) != mpc2000xl_all_t::_values_click_output_t.end();
}
const std::set<mpc2000xl_all_t::controller_t> mpc2000xl_all_t::_values_controller_t{
    mpc2000xl_all_t::CONTROLLER_BANK_SEL_MSB,
    mpc2000xl_all_t::CONTROLLER_MOD_WHEEL,
    mpc2000xl_all_t::CONTROLLER_BREATH_CONT,
    mpc2000xl_all_t::CONTROLLER_CC3,
    mpc2000xl_all_t::CONTROLLER_FOOT_CONTROL,
    mpc2000xl_all_t::CONTROLLER_PORTA_TIME,
    mpc2000xl_all_t::CONTROLLER_DATA_ENTRY,
    mpc2000xl_all_t::CONTROLLER_MAIN_VOLUME,
    mpc2000xl_all_t::CONTROLLER_BALANCE,
    mpc2000xl_all_t::CONTROLLER_CC9,
    mpc2000xl_all_t::CONTROLLER_PAN,
    mpc2000xl_all_t::CONTROLLER_EXPRESSION,
    mpc2000xl_all_t::CONTROLLER_EFFECT_1,
    mpc2000xl_all_t::CONTROLLER_EFFECT_2,
    mpc2000xl_all_t::CONTROLLER_CC14,
    mpc2000xl_all_t::CONTROLLER_CC15,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_1,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_2,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_3,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_4,
    mpc2000xl_all_t::CONTROLLER_CC20,
    mpc2000xl_all_t::CONTROLLER_CC21,
    mpc2000xl_all_t::CONTROLLER_CC22,
    mpc2000xl_all_t::CONTROLLER_CC23,
    mpc2000xl_all_t::CONTROLLER_CC24,
    mpc2000xl_all_t::CONTROLLER_CC25,
    mpc2000xl_all_t::CONTROLLER_CC26,
    mpc2000xl_all_t::CONTROLLER_CC27,
    mpc2000xl_all_t::CONTROLLER_CC28,
    mpc2000xl_all_t::CONTROLLER_CC29,
    mpc2000xl_all_t::CONTROLLER_CC30,
    mpc2000xl_all_t::CONTROLLER_CC31,
    mpc2000xl_all_t::CONTROLLER_BANK_SEL_LSB,
    mpc2000xl_all_t::CONTROLLER_MOD_WHEL_LSB,
    mpc2000xl_all_t::CONTROLLER_BREATH_LSB,
    mpc2000xl_all_t::CONTROLLER_CC35,
    mpc2000xl_all_t::CONTROLLER_FOOT_CNT_LSB,
    mpc2000xl_all_t::CONTROLLER_PORT_TIME_LS,
    mpc2000xl_all_t::CONTROLLER_DATA_ENT_LSB,
    mpc2000xl_all_t::CONTROLLER_MAIN_VOL_LSB,
    mpc2000xl_all_t::CONTROLLER_BALANCE_LSB,
    mpc2000xl_all_t::CONTROLLER_CC41,
    mpc2000xl_all_t::CONTROLLER_PAN_LSB,
    mpc2000xl_all_t::CONTROLLER_EXPRESS_LSB,
    mpc2000xl_all_t::CONTROLLER_EFFECT_1_LSB,
    mpc2000xl_all_t::CONTROLLER_EFFECT_2_MSB,
    mpc2000xl_all_t::CONTROLLER_CC46,
    mpc2000xl_all_t::CONTROLLER_CC47,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_1_LS,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_2_LS,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_3_LS,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_4_LS,
    mpc2000xl_all_t::CONTROLLER_CC52,
    mpc2000xl_all_t::CONTROLLER_CC53,
    mpc2000xl_all_t::CONTROLLER_CC54,
    mpc2000xl_all_t::CONTROLLER_CC55,
    mpc2000xl_all_t::CONTROLLER_CC56,
    mpc2000xl_all_t::CONTROLLER_CC57,
    mpc2000xl_all_t::CONTROLLER_CC58,
    mpc2000xl_all_t::CONTROLLER_CC59,
    mpc2000xl_all_t::CONTROLLER_CC60,
    mpc2000xl_all_t::CONTROLLER_CC61,
    mpc2000xl_all_t::CONTROLLER_CC62,
    mpc2000xl_all_t::CONTROLLER_CC63,
    mpc2000xl_all_t::CONTROLLER_SUSTAIN_PDL,
    mpc2000xl_all_t::CONTROLLER_PORTA_PEDAL,
    mpc2000xl_all_t::CONTROLLER_SOSTENUTO,
    mpc2000xl_all_t::CONTROLLER_SOFT_PEDAL,
    mpc2000xl_all_t::CONTROLLER_LEGATO_FT_SW,
    mpc2000xl_all_t::CONTROLLER_HOLD_2,
    mpc2000xl_all_t::CONTROLLER_SOUND_VARI,
    mpc2000xl_all_t::CONTROLLER_TIMBER_HARMO,
    mpc2000xl_all_t::CONTROLLER_RELEASE_TIME,
    mpc2000xl_all_t::CONTROLLER_ATTACK_TIME,
    mpc2000xl_all_t::CONTROLLER_BRIGHTNESS,
    mpc2000xl_all_t::CONTROLLER_SOUND_CONT_6,
    mpc2000xl_all_t::CONTROLLER_SOUND_CONT_7,
    mpc2000xl_all_t::CONTROLLER_SOUND_CONT_8,
    mpc2000xl_all_t::CONTROLLER_SOUND_CONT_9,
    mpc2000xl_all_t::CONTROLLER_SOUND_CONT10,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_5,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_6,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_7,
    mpc2000xl_all_t::CONTROLLER_GEN_PUR_8,
    mpc2000xl_all_t::CONTROLLER_PORTA_CNTRL,
    mpc2000xl_all_t::CONTROLLER_CC85,
    mpc2000xl_all_t::CONTROLLER_CC86,
    mpc2000xl_all_t::CONTROLLER_CC87,
    mpc2000xl_all_t::CONTROLLER_CC88,
    mpc2000xl_all_t::CONTROLLER_CC89,
    mpc2000xl_all_t::CONTROLLER_CC90,
    mpc2000xl_all_t::CONTROLLER_EXT_EFF_DPTH,
    mpc2000xl_all_t::CONTROLLER_TREMOLO_DPTH,
    mpc2000xl_all_t::CONTROLLER_CHORUS_DEPTH,
    mpc2000xl_all_t::CONTROLLER_DETUNE_DEPTH,
    mpc2000xl_all_t::CONTROLLER_PHASER_DEPTH,
    mpc2000xl_all_t::CONTROLLER_DATA_INCRE,
    mpc2000xl_all_t::CONTROLLER_DATA_DECRE,
    mpc2000xl_all_t::CONTROLLER_NRPN_LSB,
    mpc2000xl_all_t::CONTROLLER_NRPN_MSB,
    mpc2000xl_all_t::CONTROLLER_RPN_LSB,
    mpc2000xl_all_t::CONTROLLER_RPN_MSB,
    mpc2000xl_all_t::CONTROLLER_CC102,
    mpc2000xl_all_t::CONTROLLER_CC103,
    mpc2000xl_all_t::CONTROLLER_CC104,
    mpc2000xl_all_t::CONTROLLER_CC105,
    mpc2000xl_all_t::CONTROLLER_CC106,
    mpc2000xl_all_t::CONTROLLER_CC107,
    mpc2000xl_all_t::CONTROLLER_CC108,
    mpc2000xl_all_t::CONTROLLER_CC109,
    mpc2000xl_all_t::CONTROLLER_CC110,
    mpc2000xl_all_t::CONTROLLER_CC111,
    mpc2000xl_all_t::CONTROLLER_CC112,
    mpc2000xl_all_t::CONTROLLER_CC113,
    mpc2000xl_all_t::CONTROLLER_CC114,
    mpc2000xl_all_t::CONTROLLER_CC115,
    mpc2000xl_all_t::CONTROLLER_CC116,
    mpc2000xl_all_t::CONTROLLER_CC117,
    mpc2000xl_all_t::CONTROLLER_CC118,
    mpc2000xl_all_t::CONTROLLER_CC119,
    mpc2000xl_all_t::CONTROLLER_ALL_SND_OFF,
    mpc2000xl_all_t::CONTROLLER_RESET_CONTRL,
    mpc2000xl_all_t::CONTROLLER_LOCAL_ON_OFF,
    mpc2000xl_all_t::CONTROLLER_ALL_NOTE_OFF,
    mpc2000xl_all_t::CONTROLLER_OMNI_OFF,
    mpc2000xl_all_t::CONTROLLER_OMNI_ON,
    mpc2000xl_all_t::CONTROLLER_MONO_MODE_ON,
    mpc2000xl_all_t::CONTROLLER_POLY_MODE_ON,
};
bool mpc2000xl_all_t::_is_defined_controller_t(mpc2000xl_all_t::controller_t v) {
    return mpc2000xl_all_t::_values_controller_t.find(v) != mpc2000xl_all_t::_values_controller_t.end();
}
const std::set<mpc2000xl_all_t::count_in_mode_t> mpc2000xl_all_t::_values_count_in_mode_t{
    mpc2000xl_all_t::COUNT_IN_MODE_FALSE,
    mpc2000xl_all_t::COUNT_IN_MODE_REC_ONLY,
    mpc2000xl_all_t::COUNT_IN_MODE_REC_AND_PLAY,
};
bool mpc2000xl_all_t::_is_defined_count_in_mode_t(mpc2000xl_all_t::count_in_mode_t v) {
    return mpc2000xl_all_t::_values_count_in_mode_t.find(v) != mpc2000xl_all_t::_values_count_in_mode_t.end();
}
const std::set<mpc2000xl_all_t::duration_of_recorded_notes_t> mpc2000xl_all_t::_values_duration_of_recorded_notes_t{
    mpc2000xl_all_t::DURATION_OF_RECORDED_NOTES_AS_PLAYED,
    mpc2000xl_all_t::DURATION_OF_RECORDED_NOTES_TC_VALUE,
};
bool mpc2000xl_all_t::_is_defined_duration_of_recorded_notes_t(mpc2000xl_all_t::duration_of_recorded_notes_t v) {
    return mpc2000xl_all_t::_values_duration_of_recorded_notes_t.find(v) != mpc2000xl_all_t::_values_duration_of_recorded_notes_t.end();
}
const std::set<mpc2000xl_all_t::frame_rate_t> mpc2000xl_all_t::_values_frame_rate_t{
    mpc2000xl_all_t::FRAME_RATE_FPS_24,
    mpc2000xl_all_t::FRAME_RATE_FPS_25,
    mpc2000xl_all_t::FRAME_RATE_FPS_30D,
    mpc2000xl_all_t::FRAME_RATE_FPS_30,
};
bool mpc2000xl_all_t::_is_defined_frame_rate_t(mpc2000xl_all_t::frame_rate_t v) {
    return mpc2000xl_all_t::_values_frame_rate_t.find(v) != mpc2000xl_all_t::_values_frame_rate_t.end();
}
const std::set<mpc2000xl_all_t::midi_input_filter_type_t> mpc2000xl_all_t::_values_midi_input_filter_type_t{
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_NOTES,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_PITCH_BEND,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_PROG_CHANGE,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CH_PRESSURE,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_POLY_PRESS,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_EXCLUSIVE,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC0_BANK_SEL_MSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC1_MOD_WHEEL,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC2_BREATH_CONT,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC3,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC4_FOOT_CONTROL,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC5_PORTA_TIME,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC6_DATA_ENTRY,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC7_MAIN_VOLUME,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC8_BALANCE,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC9,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC10_PAN,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC11_EXPRESSION,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC12_EFFECT_1,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC13_EFFECT_2,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC14,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC15,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC16_GEN_PUR_1,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC17_GEN_PUR_2,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC18_GEN_PUR_3,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC19_GEN_PUR_4,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC20,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC21,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC22,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC23,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC24,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC25,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC26,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC27,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC28,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC29,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC30,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC31,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC32_BANK_SEL_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC33_MOD_WHEL_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC34_BREATH_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC35,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC36_FOOT_CNT_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC37_PORT_TIME_LS,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC38_DATA_ENT_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC39_MAIN_VOL_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC40_BALANCE_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC41,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC42_PAN_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC43_EXPRESS_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC44_EFFECT_1_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC45_EFFECT_2_MSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC46,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC47,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC48_GEN_PUR_1_LS,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC49_GEN_PUR_2_LS,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC50_GEN_PUR_3_LS,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC51_GEN_PUR_4_LS,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC52,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC53,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC54,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC55,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC56,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC57,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC58,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC59,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC60,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC61,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC62,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC63,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC64_SUSTAIN_PDL,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC65_PORTA_PEDAL,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC66_SOSTENUTO,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC67_SOFT_PEDAL,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC68_LEGATO_FT_SW,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC69_HOLD_2,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC70_SOUND_VARI,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC71_TIMBER_HARMO,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC72_RELEASE_TIME,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC73_ATTACK_TIME,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC74_BRIGHTNESS,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC75_SOUND_CONT_6,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC76_SOUND_CONT_7,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC77_SOUND_CONT_8,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC78_SOUND_CONT_9,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC79_SOUND_CONT10,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC80_GEN_PUR_5,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC81_GEN_PUR_6,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC82_GEN_PUR_7,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC83_GEN_PUR_8,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC84_PORTA_CNTRL,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC85,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC86,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC87,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC88,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC89,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC90,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC91_EXT_EFF_DPTH,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC92_TREMOLO_DPTH,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC93_CHORUS_DEPTH,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC94_DETUNE_DEPTH,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC95_PHASER_DEPTH,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC96_DATA_INCRE,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC97_DATA_DECRE,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC98_NRPN_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC99_NRPN_MSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC100_RPN_LSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC101_RPN_MSB,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC102,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC103,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC104,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC105,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC106,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC107,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC108,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC109,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC110,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC111,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC112,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC113,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC114,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC115,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC116,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC117,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC118,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC119,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC120_ALL_SND_OFF,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC121_RESET_CONTRL,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC122_LOCAL_ON_OFF,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC123_ALL_NOTE_OFF,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC124_OMNI_OFF,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC125_OMNI_ON,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC126_MONO_MODE_ON,
    mpc2000xl_all_t::MIDI_INPUT_FILTER_TYPE_CC127_POLY_MODE_ON,
};
bool mpc2000xl_all_t::_is_defined_midi_input_filter_type_t(mpc2000xl_all_t::midi_input_filter_type_t v) {
    return mpc2000xl_all_t::_values_midi_input_filter_type_t.find(v) != mpc2000xl_all_t::_values_midi_input_filter_type_t.end();
}
const std::set<mpc2000xl_all_t::midi_input_receive_channel_t> mpc2000xl_all_t::_values_midi_input_receive_channel_t{
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_ALL,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH1,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH2,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH3,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH4,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH5,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH6,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH7,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH8,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH9,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH10,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH11,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH12,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH13,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH14,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH15,
    mpc2000xl_all_t::MIDI_INPUT_RECEIVE_CHANNEL_CH16,
};
bool mpc2000xl_all_t::_is_defined_midi_input_receive_channel_t(mpc2000xl_all_t::midi_input_receive_channel_t v) {
    return mpc2000xl_all_t::_values_midi_input_receive_channel_t.find(v) != mpc2000xl_all_t::_values_midi_input_receive_channel_t.end();
}
const std::set<mpc2000xl_all_t::midi_switch_function_t> mpc2000xl_all_t::_values_midi_switch_function_t{
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PLAY_STRT,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PLAY,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_STOP,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_REC_AND_PLAY,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_ODUB_AND_PLAY,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_REC_PUNCH,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_ODUB_PNCH,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_TAP,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_BNK_A,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_BNK_B,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_BNK_C,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_BNK_D,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_1,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_2,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_3,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_4,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_5,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_6,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_7,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_8,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_9,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_10,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_11,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_12,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_13,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_14,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_15,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_PAD_16,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_F1,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_F2,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_F3,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_F4,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_F5,
    mpc2000xl_all_t::MIDI_SWITCH_FUNCTION_F6,
};
bool mpc2000xl_all_t::_is_defined_midi_switch_function_t(mpc2000xl_all_t::midi_switch_function_t v) {
    return mpc2000xl_all_t::_values_midi_switch_function_t.find(v) != mpc2000xl_all_t::_values_midi_switch_function_t.end();
}
const std::set<mpc2000xl_all_t::midi_sync_mode_t> mpc2000xl_all_t::_values_midi_sync_mode_t{
    mpc2000xl_all_t::MIDI_SYNC_MODE_FALSE,
    mpc2000xl_all_t::MIDI_SYNC_MODE_MIDI_CLOCK,
    mpc2000xl_all_t::MIDI_SYNC_MODE_TIME_CODE,
};
bool mpc2000xl_all_t::_is_defined_midi_sync_mode_t(mpc2000xl_all_t::midi_sync_mode_t v) {
    return mpc2000xl_all_t::_values_midi_sync_mode_t.find(v) != mpc2000xl_all_t::_values_midi_sync_mode_t.end();
}
const std::set<mpc2000xl_all_t::midi_sync_output_t> mpc2000xl_all_t::_values_midi_sync_output_t{
    mpc2000xl_all_t::MIDI_SYNC_OUTPUT_A,
    mpc2000xl_all_t::MIDI_SYNC_OUTPUT_B,
    mpc2000xl_all_t::MIDI_SYNC_OUTPUT_A_AND_B,
};
bool mpc2000xl_all_t::_is_defined_midi_sync_output_t(mpc2000xl_all_t::midi_sync_output_t v) {
    return mpc2000xl_all_t::_values_midi_sync_output_t.find(v) != mpc2000xl_all_t::_values_midi_sync_output_t.end();
}
const std::set<mpc2000xl_all_t::mixer_event_param_t> mpc2000xl_all_t::_values_mixer_event_param_t{
    mpc2000xl_all_t::MIXER_EVENT_PARAM_STEREO_LEVEL,
    mpc2000xl_all_t::MIXER_EVENT_PARAM_STEREO_PAN,
    mpc2000xl_all_t::MIXER_EVENT_PARAM_FXSEND_LEVEL,
    mpc2000xl_all_t::MIXER_EVENT_PARAM_INDIV_LEVEL,
};
bool mpc2000xl_all_t::_is_defined_mixer_event_param_t(mpc2000xl_all_t::mixer_event_param_t v) {
    return mpc2000xl_all_t::_values_mixer_event_param_t.find(v) != mpc2000xl_all_t::_values_mixer_event_param_t.end();
}
const std::set<mpc2000xl_all_t::off_on_t> mpc2000xl_all_t::_values_off_on_t{
    mpc2000xl_all_t::OFF_ON_FALSE,
    mpc2000xl_all_t::OFF_ON_TRUE,
};
bool mpc2000xl_all_t::_is_defined_off_on_t(mpc2000xl_all_t::off_on_t v) {
    return mpc2000xl_all_t::_values_off_on_t.find(v) != mpc2000xl_all_t::_values_off_on_t.end();
}
const std::set<mpc2000xl_all_t::rate_t> mpc2000xl_all_t::_values_rate_t{
    mpc2000xl_all_t::RATE_ONE_4,
    mpc2000xl_all_t::RATE_ONE_4_3,
    mpc2000xl_all_t::RATE_ONE_8,
    mpc2000xl_all_t::RATE_ONE_8_3,
    mpc2000xl_all_t::RATE_ONE_16,
    mpc2000xl_all_t::RATE_ONE_16_3,
    mpc2000xl_all_t::RATE_ONE_32,
    mpc2000xl_all_t::RATE_ONE_32_3,
};
bool mpc2000xl_all_t::_is_defined_rate_t(mpc2000xl_all_t::rate_t v) {
    return mpc2000xl_all_t::_values_rate_t.find(v) != mpc2000xl_all_t::_values_rate_t.end();
}
const std::set<mpc2000xl_all_t::sequence_is_used_t> mpc2000xl_all_t::_values_sequence_is_used_t{
    mpc2000xl_all_t::SEQUENCE_IS_USED_FALSE,
    mpc2000xl_all_t::SEQUENCE_IS_USED_TRUE,
};
bool mpc2000xl_all_t::_is_defined_sequence_is_used_t(mpc2000xl_all_t::sequence_is_used_t v) {
    return mpc2000xl_all_t::_values_sequence_is_used_t.find(v) != mpc2000xl_all_t::_values_sequence_is_used_t.end();
}
const std::set<mpc2000xl_all_t::soft_thru_mode_t> mpc2000xl_all_t::_values_soft_thru_mode_t{
    mpc2000xl_all_t::SOFT_THRU_MODE_FALSE,
    mpc2000xl_all_t::SOFT_THRU_MODE_AS_TRACK,
    mpc2000xl_all_t::SOFT_THRU_MODE_OMNI_A,
    mpc2000xl_all_t::SOFT_THRU_MODE_OMNI_B,
    mpc2000xl_all_t::SOFT_THRU_MODE_OMNI_AB,
};
bool mpc2000xl_all_t::_is_defined_soft_thru_mode_t(mpc2000xl_all_t::soft_thru_mode_t v) {
    return mpc2000xl_all_t::_values_soft_thru_mode_t.find(v) != mpc2000xl_all_t::_values_soft_thru_mode_t.end();
}
const std::set<mpc2000xl_all_t::sound_source_t> mpc2000xl_all_t::_values_sound_source_t{
    mpc2000xl_all_t::SOUND_SOURCE_CLICK,
    mpc2000xl_all_t::SOUND_SOURCE_DRUM1,
    mpc2000xl_all_t::SOUND_SOURCE_DRUM2,
    mpc2000xl_all_t::SOUND_SOURCE_DRUM3,
    mpc2000xl_all_t::SOUND_SOURCE_DRUM4,
};
bool mpc2000xl_all_t::_is_defined_sound_source_t(mpc2000xl_all_t::sound_source_t v) {
    return mpc2000xl_all_t::_values_sound_source_t.find(v) != mpc2000xl_all_t::_values_sound_source_t.end();
}
const std::set<mpc2000xl_all_t::tap_averaging_t> mpc2000xl_all_t::_values_tap_averaging_t{
    mpc2000xl_all_t::TAP_AVERAGING_TAP_AVG_2,
    mpc2000xl_all_t::TAP_AVERAGING_TAP_AVG_3,
    mpc2000xl_all_t::TAP_AVERAGING_TAP_AVG_4,
};
bool mpc2000xl_all_t::_is_defined_tap_averaging_t(mpc2000xl_all_t::tap_averaging_t v) {
    return mpc2000xl_all_t::_values_tap_averaging_t.find(v) != mpc2000xl_all_t::_values_tap_averaging_t.end();
}
const std::set<mpc2000xl_all_t::time_display_style_t> mpc2000xl_all_t::_values_time_display_style_t{
    mpc2000xl_all_t::TIME_DISPLAY_STYLE_BAR_BEAT_CLOCK,
    mpc2000xl_all_t::TIME_DISPLAY_STYLE_HOUR_MINUTE_SEC,
};
bool mpc2000xl_all_t::_is_defined_time_display_style_t(mpc2000xl_all_t::time_display_style_t v) {
    return mpc2000xl_all_t::_values_time_display_style_t.find(v) != mpc2000xl_all_t::_values_time_display_style_t.end();
}
const std::set<mpc2000xl_all_t::timing_correct_t> mpc2000xl_all_t::_values_timing_correct_t{
    mpc2000xl_all_t::TIMING_CORRECT_FALSE,
    mpc2000xl_all_t::TIMING_CORRECT_ONE_8,
    mpc2000xl_all_t::TIMING_CORRECT_ONE_8_3,
    mpc2000xl_all_t::TIMING_CORRECT_ONE_16,
    mpc2000xl_all_t::TIMING_CORRECT_ONE_16_3,
    mpc2000xl_all_t::TIMING_CORRECT_ONE_32,
    mpc2000xl_all_t::TIMING_CORRECT_ONE_32_3,
};
bool mpc2000xl_all_t::_is_defined_timing_correct_t(mpc2000xl_all_t::timing_correct_t v) {
    return mpc2000xl_all_t::_values_timing_correct_t.find(v) != mpc2000xl_all_t::_values_timing_correct_t.end();
}
const std::set<mpc2000xl_all_t::unused_used_t> mpc2000xl_all_t::_values_unused_used_t{
    mpc2000xl_all_t::UNUSED_USED_UNUSED,
    mpc2000xl_all_t::UNUSED_USED_USED,
};
bool mpc2000xl_all_t::_is_defined_unused_used_t(mpc2000xl_all_t::unused_used_t v) {
    return mpc2000xl_all_t::_values_unused_used_t.find(v) != mpc2000xl_all_t::_values_unused_used_t.end();
}

mpc2000xl_all_t::mpc2000xl_all_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc2000xl_all_t::_read() {
    m_magic = m__io->read_bytes(16);
    if (!(m_magic == std::string("\x4D\x50\x43\x32\x4B\x58\x4C\x20\x41\x4C\x4C\x20\x31\x2E\x30\x30", 16))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4D\x50\x43\x32\x4B\x58\x4C\x20\x41\x4C\x4C\x20\x31\x2E\x30\x30", 16), m_magic, m__io, std::string("/seq/0"));
    }
    m_defaults = std::unique_ptr<defaults_t>(new defaults_t(m__io, this, m__root));
    m_defaults->_read();
    m_sequencer = std::unique_ptr<sequencer_t>(new sequencer_t(m__io, this, m__root));
    m_sequencer->_read();
    m__unnamed3 = m__io->read_bytes(10);
    m_count = std::unique_ptr<count_t>(new count_t(m__io, this, m__root));
    m_count->_read();
    m_midi_output = std::unique_ptr<midi_output_t>(new midi_output_t(m__io, this, m__root));
    m_midi_output->_read();
    m_midi_input = std::unique_ptr<midi_input_t>(new midi_input_t(m__io, this, m__root));
    m_midi_input->_read();
    m_midi_sync = std::unique_ptr<midi_sync_t>(new midi_sync_t(m__io, this, m__root));
    m_midi_sync->_read();
    m_song_global = std::unique_ptr<song_global_t>(new song_global_t(m__io, this, m__root));
    m_song_global->_read();
    m__unnamed9 = m__io->read_bytes(5);
    m_locations = std::unique_ptr<std::vector<std::unique_ptr<location_t>>>(new std::vector<std::unique_ptr<location_t>>());
    const int l_locations = 9;
    for (int i = 0; i < l_locations; i++) {
        std::unique_ptr<location_t> _t_locations = std::unique_ptr<location_t>(new location_t(m__io, this, m__root));
        try {
            _t_locations->_read();
        } catch(...) {
            m_locations->push_back(std::move(_t_locations));
            throw;
        }
        m_locations->push_back(std::move(_t_locations));
    }
    m_misc = std::unique_ptr<misc_t>(new misc_t(m__io, this, m__root));
    m_misc->_read();
    m__unnamed12 = m__io->read_bytes(3);
    m_step_edit_options = std::unique_ptr<step_edit_options_t>(new step_edit_options_t(m__io, this, m__root));
    m_step_edit_options->_read();
    m_prog_change_to_seq = m__io->read_bits_int_le(1);
    m__unnamed15 = m__io->read_bytes(78);
    m_sequences_metas = std::unique_ptr<std::vector<std::unique_ptr<sequence_meta_t>>>(new std::vector<std::unique_ptr<sequence_meta_t>>());
    const int l_sequences_metas = 99;
    for (int i = 0; i < l_sequences_metas; i++) {
        std::unique_ptr<sequence_meta_t> _t_sequences_metas = std::unique_ptr<sequence_meta_t>(new sequence_meta_t(m__io, this, m__root));
        try {
            _t_sequences_metas->_read();
        } catch(...) {
            m_sequences_metas->push_back(std::move(_t_sequences_metas));
            throw;
        }
        m_sequences_metas->push_back(std::move(_t_sequences_metas));
    }
    m_songs = std::unique_ptr<std::vector<std::unique_ptr<song_t>>>(new std::vector<std::unique_ptr<song_t>>());
    const int l_songs = 20;
    for (int i = 0; i < l_songs; i++) {
        std::unique_ptr<song_t> _t_songs = std::unique_ptr<song_t>(new song_t(m__io, this, m__root));
        try {
            _t_songs->_read();
        } catch(...) {
            m_songs->push_back(std::move(_t_songs));
            throw;
        }
        m_songs->push_back(std::move(_t_songs));
    }
    m_sequences = std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>>(new std::vector<std::unique_ptr<sequence_t>>());
    {
        int i = 0;
        while (!m__io->is_eof()) {
            std::unique_ptr<sequence_t> _t_sequences = std::unique_ptr<sequence_t>(new sequence_t(m__io, this, m__root));
            try {
                _t_sequences->_read();
            } catch(...) {
                m_sequences->push_back(std::move(_t_sequences));
                throw;
            }
            m_sequences->push_back(std::move(_t_sequences));
            i++;
        }
    }
    m__dirty = false;
}

void mpc2000xl_all_t::_fetch_instances() {
    m_defaults.get()->_fetch_instances();
    m_sequencer.get()->_fetch_instances();
    m_count.get()->_fetch_instances();
    m_midi_output.get()->_fetch_instances();
    m_midi_input.get()->_fetch_instances();
    m_midi_sync.get()->_fetch_instances();
    m_song_global.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_locations->size(); ++i) {
        m_locations->at(i).get()->_fetch_instances();
    }
    m_misc.get()->_fetch_instances();
    m_step_edit_options.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_sequences_metas->size(); ++i) {
        m_sequences_metas->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_songs->size(); ++i) {
        m_songs->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_sequences->size(); ++i) {
        m_sequences->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::_write() {
    m__io->write_bytes(m_magic);
    if (m_defaults.get() == nullptr) {
        throw std::runtime_error("/seq/1: nested object is not set");
    }
    m_defaults.get()->_set_io(m__io);
    m_defaults.get()->_write();
    if (m_sequencer.get() == nullptr) {
        throw std::runtime_error("/seq/2: nested object is not set");
    }
    m_sequencer.get()->_set_io(m__io);
    m_sequencer.get()->_write();
    m__io->write_bytes(m__unnamed3);
    if (m_count.get() == nullptr) {
        throw std::runtime_error("/seq/4: nested object is not set");
    }
    m_count.get()->_set_io(m__io);
    m_count.get()->_write();
    if (m_midi_output.get() == nullptr) {
        throw std::runtime_error("/seq/5: nested object is not set");
    }
    m_midi_output.get()->_set_io(m__io);
    m_midi_output.get()->_write();
    if (m_midi_input.get() == nullptr) {
        throw std::runtime_error("/seq/6: nested object is not set");
    }
    m_midi_input.get()->_set_io(m__io);
    m_midi_input.get()->_write();
    if (m_midi_sync.get() == nullptr) {
        throw std::runtime_error("/seq/7: nested object is not set");
    }
    m_midi_sync.get()->_set_io(m__io);
    m_midi_sync.get()->_write();
    if (m_song_global.get() == nullptr) {
        throw std::runtime_error("/seq/8: nested object is not set");
    }
    m_song_global.get()->_set_io(m__io);
    m_song_global.get()->_write();
    m__io->write_bytes(m__unnamed9);
    if (m_locations == nullptr) {
        throw std::runtime_error("/seq/10: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<location_t>>::const_iterator it = m_locations->begin(); it != m_locations->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/10: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_misc.get() == nullptr) {
        throw std::runtime_error("/seq/11: nested object is not set");
    }
    m_misc.get()->_set_io(m__io);
    m_misc.get()->_write();
    m__io->write_bytes(m__unnamed12);
    if (m_step_edit_options.get() == nullptr) {
        throw std::runtime_error("/seq/13: nested object is not set");
    }
    m_step_edit_options.get()->_set_io(m__io);
    m_step_edit_options.get()->_write();
    m__io->write_bits_int_le(1, ((m_prog_change_to_seq) ? 1 : 0));
    m__io->write_bytes(m__unnamed15);
    if (m_sequences_metas == nullptr) {
        throw std::runtime_error("/seq/16: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<sequence_meta_t>>::const_iterator it = m_sequences_metas->begin(); it != m_sequences_metas->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/16: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_songs == nullptr) {
        throw std::runtime_error("/seq/17: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<song_t>>::const_iterator it = m_songs->begin(); it != m_songs->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/17: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_sequences == nullptr) {
        throw std::runtime_error("/seq/18: repeated field is not set");
    }
    if (m_sequences == nullptr) {
        throw std::runtime_error("/seq/18: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<sequence_t>>::const_iterator it = m_sequences->begin(); it != m_sequences->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/18: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::_check() {
    if (m_magic.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_magic == std::string("\x4D\x50\x43\x32\x4B\x58\x4C\x20\x41\x4C\x4C\x20\x31\x2E\x30\x30", 16))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4D\x50\x43\x32\x4B\x58\x4C\x20\x41\x4C\x4C\x20\x31\x2E\x30\x30", 16), m_magic, m__io, std::string("/seq/0"));
    }
    if (m_defaults.get() == nullptr) {
        throw std::runtime_error("/seq/1: nested object is not set");
    }
    m_defaults.get()->_set_io(m__io);
    m_defaults.get()->_check();
    if (m_sequencer.get() == nullptr) {
        throw std::runtime_error("/seq/2: nested object is not set");
    }
    m_sequencer.get()->_set_io(m__io);
    m_sequencer.get()->_check();
    if (m__unnamed3.size() != static_cast<std::string::size_type>(10)) {
        throw std::runtime_error("/seq/3: size mismatch");
    }
    if (m_count.get() == nullptr) {
        throw std::runtime_error("/seq/4: nested object is not set");
    }
    m_count.get()->_set_io(m__io);
    m_count.get()->_check();
    if (m_midi_output.get() == nullptr) {
        throw std::runtime_error("/seq/5: nested object is not set");
    }
    m_midi_output.get()->_set_io(m__io);
    m_midi_output.get()->_check();
    if (m_midi_input.get() == nullptr) {
        throw std::runtime_error("/seq/6: nested object is not set");
    }
    m_midi_input.get()->_set_io(m__io);
    m_midi_input.get()->_check();
    if (m_midi_sync.get() == nullptr) {
        throw std::runtime_error("/seq/7: nested object is not set");
    }
    m_midi_sync.get()->_set_io(m__io);
    m_midi_sync.get()->_check();
    if (m_song_global.get() == nullptr) {
        throw std::runtime_error("/seq/8: nested object is not set");
    }
    m_song_global.get()->_set_io(m__io);
    m_song_global.get()->_check();
    if (m__unnamed9.size() != static_cast<std::string::size_type>(5)) {
        throw std::runtime_error("/seq/9: size mismatch");
    }
    if (m_locations == nullptr) {
        throw std::runtime_error("/seq/10: repeated field is not set");
    }
    if (m_locations->size() != static_cast<std::size_t>(9)) {
        throw std::runtime_error("/seq/10: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<location_t>>::const_iterator it = m_locations->begin(); it != m_locations->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/10: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_misc.get() == nullptr) {
        throw std::runtime_error("/seq/11: nested object is not set");
    }
    m_misc.get()->_set_io(m__io);
    m_misc.get()->_check();
    if (m__unnamed12.size() != static_cast<std::string::size_type>(3)) {
        throw std::runtime_error("/seq/12: size mismatch");
    }
    if (m_step_edit_options.get() == nullptr) {
        throw std::runtime_error("/seq/13: nested object is not set");
    }
    m_step_edit_options.get()->_set_io(m__io);
    m_step_edit_options.get()->_check();
    if (m__unnamed15.size() != static_cast<std::string::size_type>(78)) {
        throw std::runtime_error("/seq/15: size mismatch");
    }
    if (m_sequences_metas == nullptr) {
        throw std::runtime_error("/seq/16: repeated field is not set");
    }
    if (m_sequences_metas->size() != static_cast<std::size_t>(99)) {
        throw std::runtime_error("/seq/16: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<sequence_meta_t>>::const_iterator it = m_sequences_metas->begin(); it != m_sequences_metas->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/16: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_songs == nullptr) {
        throw std::runtime_error("/seq/17: repeated field is not set");
    }
    if (m_songs->size() != static_cast<std::size_t>(20)) {
        throw std::runtime_error("/seq/17: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<song_t>>::const_iterator it = m_songs->begin(); it != m_songs->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/17: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_sequences == nullptr) {
        throw std::runtime_error("/seq/18: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<sequence_t>>::const_iterator it = m_sequences->begin(); it != m_sequences->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_sequences->begin());
        const sequence_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/18: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

mpc2000xl_all_t::~mpc2000xl_all_t() {}

mpc2000xl_all_t::bar_t::bar_t(int32_t p_idx, kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_idx = p_idx;
    m__dirty = false;
    f_denominator = false;
    f_first_tick = false;
    f_numerator = false;
}

void mpc2000xl_all_t::bar_t::_read() {
    m_ticks_per_beat = m__io->read_u1();
    m_last_tick = m__io->read_bits_int_le(24);
    m__dirty = false;
}

void mpc2000xl_all_t::bar_t::_fetch_instances() {
}

void mpc2000xl_all_t::bar_t::_write() {
    m__io->write_u1(m_ticks_per_beat);
    m__io->write_bits_int_le(24, m_last_tick);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::bar_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::bar_t::~bar_t() {}

int32_t mpc2000xl_all_t::bar_t::denominator() {
    if (f_denominator)
        return m_denominator;
    f_denominator = true;
    m_denominator = 4 / (ticks_per_beat() / 96);
    return m_denominator;
}

int32_t mpc2000xl_all_t::bar_t::first_tick() {
    if (f_first_tick)
        return m_first_tick;
    f_first_tick = true;
    m_first_tick = ((idx() == 0) ? (0) : (static_cast<mpc2000xl_all_t::sequence_body_t*>(_parent())->bars()->at(idx() - 1)->last_tick()));
    return m_first_tick;
}

int32_t mpc2000xl_all_t::bar_t::numerator() {
    if (f_numerator)
        return m_numerator;
    f_numerator = true;
    m_numerator = (last_tick() - first_tick()) / ticks_per_beat();
    return m_numerator;
}

mpc2000xl_all_t::ch_pressure_event_t::ch_pressure_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::ch_pressure_event_t::_read() {
    m_pressure = m__io->read_u1();
    if (!(m_pressure >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_pressure, m__io, std::string("/types/ch_pressure_event/seq/0"));
    }
    if (!(m_pressure <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_pressure, m__io, std::string("/types/ch_pressure_event/seq/0"));
    }
    m__unnamed1 = m__io->read_bytes(2);
    m__dirty = false;
}

void mpc2000xl_all_t::ch_pressure_event_t::_fetch_instances() {
}

void mpc2000xl_all_t::ch_pressure_event_t::_write() {
    m__io->write_u1(m_pressure);
    m__io->write_bytes(m__unnamed1);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::ch_pressure_event_t::_check() {
    if (!(m_pressure >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_pressure, m__io, std::string("/types/ch_pressure_event/seq/0"));
    }
    if (!(m_pressure <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_pressure, m__io, std::string("/types/ch_pressure_event/seq/0"));
    }
    if (m__unnamed1.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/types/ch_pressure_event/seq/1: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::ch_pressure_event_t::~ch_pressure_event_t() {}

mpc2000xl_all_t::control_change_event_t::control_change_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::control_change_event_t::_read() {
    m_controller = static_cast<mpc2000xl_all_t::controller_t>(m__io->read_u1());
    m_value = m__io->read_u1();
    if (!(m_value >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_value, m__io, std::string("/types/control_change_event/seq/1"));
    }
    if (!(m_value <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_value, m__io, std::string("/types/control_change_event/seq/1"));
    }
    m__unnamed2 = m__io->read_bytes(1);
    m__dirty = false;
}

void mpc2000xl_all_t::control_change_event_t::_fetch_instances() {
}

void mpc2000xl_all_t::control_change_event_t::_write() {
    m__io->write_u1(static_cast<uint8_t>(m_controller));
    m__io->write_u1(m_value);
    m__io->write_bytes(m__unnamed2);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::control_change_event_t::_check() {
    if (!(m_value >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_value, m__io, std::string("/types/control_change_event/seq/1"));
    }
    if (!(m_value <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_value, m__io, std::string("/types/control_change_event/seq/1"));
    }
    if (m__unnamed2.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/control_change_event/seq/2: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::control_change_event_t::~control_change_event_t() {}

mpc2000xl_all_t::count_t::count_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::count_t::_read() {
    m_enabled = m__io->read_bits_int_le(1);
    m__unnamed1 = m__io->read_bits_int_le(7);
    m_count_in_mode = static_cast<mpc2000xl_all_t::count_in_mode_t>(m__io->read_u1());
    m_click_volume = m__io->read_u1();
    if (!(m_click_volume >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_click_volume, m__io, std::string("/types/count/seq/3"));
    }
    if (!(m_click_volume <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_click_volume, m__io, std::string("/types/count/seq/3"));
    }
    m_rate = static_cast<mpc2000xl_all_t::rate_t>(m__io->read_u1());
    m_enabled_in_play = m__io->read_bits_int_le(1);
    m__unnamed6 = m__io->read_bits_int_le(7);
    m_enabled_in_rec = m__io->read_bits_int_le(1);
    m__unnamed8 = m__io->read_bits_int_le(7);
    m_click_output = static_cast<mpc2000xl_all_t::click_output_t>(m__io->read_u1());
    m_wait_for_key = m__io->read_bits_int_le(1);
    m__unnamed11 = m__io->read_bits_int_le(7);
    m_sound_source = static_cast<mpc2000xl_all_t::sound_source_t>(m__io->read_u1());
    m_accent_pad_index = m__io->read_u1();
    m_normal_pad_index = m__io->read_u1();
    m_accent_velo = m__io->read_u1();
    if (!(m_accent_velo >= 1)) {
        throw kaitai::validation_less_than_error<uint8_t>(1, m_accent_velo, m__io, std::string("/types/count/seq/15"));
    }
    if (!(m_accent_velo <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_accent_velo, m__io, std::string("/types/count/seq/15"));
    }
    m_normal_velo = m__io->read_u1();
    if (!(m_normal_velo >= 1)) {
        throw kaitai::validation_less_than_error<uint8_t>(1, m_normal_velo, m__io, std::string("/types/count/seq/16"));
    }
    if (!(m_normal_velo <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_normal_velo, m__io, std::string("/types/count/seq/16"));
    }
    m__dirty = false;
}

void mpc2000xl_all_t::count_t::_fetch_instances() {
}

void mpc2000xl_all_t::count_t::_write() {
    m__io->write_bits_int_le(1, ((m_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed1);
    m__io->write_u1(static_cast<uint8_t>(m_count_in_mode));
    m__io->write_u1(m_click_volume);
    m__io->write_u1(static_cast<uint8_t>(m_rate));
    m__io->write_bits_int_le(1, ((m_enabled_in_play) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed6);
    m__io->write_bits_int_le(1, ((m_enabled_in_rec) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed8);
    m__io->write_u1(static_cast<uint8_t>(m_click_output));
    m__io->write_bits_int_le(1, ((m_wait_for_key) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed11);
    m__io->write_u1(static_cast<uint8_t>(m_sound_source));
    m__io->write_u1(m_accent_pad_index);
    m__io->write_u1(m_normal_pad_index);
    m__io->write_u1(m_accent_velo);
    m__io->write_u1(m_normal_velo);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::count_t::_check() {
    if (!(m_click_volume >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_click_volume, m__io, std::string("/types/count/seq/3"));
    }
    if (!(m_click_volume <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_click_volume, m__io, std::string("/types/count/seq/3"));
    }
    if (!(m_accent_velo >= 1)) {
        throw kaitai::validation_less_than_error<uint8_t>(1, m_accent_velo, m__io, std::string("/types/count/seq/15"));
    }
    if (!(m_accent_velo <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_accent_velo, m__io, std::string("/types/count/seq/15"));
    }
    if (!(m_normal_velo >= 1)) {
        throw kaitai::validation_less_than_error<uint8_t>(1, m_normal_velo, m__io, std::string("/types/count/seq/16"));
    }
    if (!(m_normal_velo <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_normal_velo, m__io, std::string("/types/count/seq/16"));
    }
    m__dirty = false;
}

mpc2000xl_all_t::count_t::~count_t() {}

mpc2000xl_all_t::defaults_t::defaults_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::defaults_t::_read() {
    m_sequence_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m__unnamed1 = m__io->read_bytes(6);
    m_tempo = m__io->read_u2le();
    m_numerator = m__io->read_u1();
    m_denominator = m__io->read_u1();
    m_bar_count = m__io->read_u2le();
    m_tick_count = m__io->read_u2le();
    m_unknown1 = std::unique_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>());
    const int l_unknown1 = 4;
    for (int i = 0; i < l_unknown1; i++) {
        m_unknown1->push_back(std::move(m__io->read_u4le()));
    }
    m_unknown2 = m__io->read_bytes(74);
    m_device_names = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    const int l_device_names = 33;
    for (int i = 0; i < l_device_names; i++) {
        m_device_names->push_back(std::move(kaitai::kstream::bytes_to_str(m__io->read_bytes(8), "ASCII")));
    }
    m_track_names = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    const int l_track_names = 64;
    for (int i = 0; i < l_track_names; i++) {
        m_track_names->push_back(std::move(kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII")));
    }
    m_devices = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_devices = 64;
    for (int i = 0; i < l_devices; i++) {
        m_devices->push_back(std::move(m__io->read_u1()));
    }
    m_buses = std::unique_ptr<std::vector<bus_t>>(new std::vector<bus_t>());
    const int l_buses = 64;
    for (int i = 0; i < l_buses; i++) {
        m_buses->push_back(std::move(static_cast<mpc2000xl_all_t::bus_t>(m__io->read_u1())));
    }
    m_programs = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_programs = 64;
    for (int i = 0; i < l_programs; i++) {
        m_programs->push_back(std::move(m__io->read_u1()));
    }
    m_track_velocities = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_track_velocities = 64;
    for (int i = 0; i < l_track_velocities; i++) {
        m_track_velocities->push_back(std::move(m__io->read_u1()));
    }
    m_track_statuses = std::unique_ptr<std::vector<std::unique_ptr<track_status_t>>>(new std::vector<std::unique_ptr<track_status_t>>());
    const int l_track_statuses = 64;
    for (int i = 0; i < l_track_statuses; i++) {
        std::unique_ptr<track_status_t> _t_track_statuses = std::unique_ptr<track_status_t>(new track_status_t(m__io, this, m__root));
        try {
            _t_track_statuses->_read();
        } catch(...) {
            m_track_statuses->push_back(std::move(_t_track_statuses));
            throw;
        }
        m_track_statuses->push_back(std::move(_t_track_statuses));
    }
    m__unnamed16 = m__io->read_bytes(64);
    m__dirty = false;
}

void mpc2000xl_all_t::defaults_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_unknown1->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_device_names->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_track_names->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_devices->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_buses->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_programs->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_track_velocities->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_track_statuses->size(); ++i) {
        m_track_statuses->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::defaults_t::_write() {
    m__io->write_bytes(m_sequence_name);
    m__io->write_bytes(m__unnamed1);
    m__io->write_u2le(m_tempo);
    m__io->write_u1(m_numerator);
    m__io->write_u1(m_denominator);
    m__io->write_u2le(m_bar_count);
    m__io->write_u2le(m_tick_count);
    if (m_unknown1 == nullptr) {
        throw std::runtime_error("/types/defaults/seq/7: repeated field is not set");
    }
    for (std::vector<uint32_t>::const_iterator it = m_unknown1->begin(); it != m_unknown1->end(); ++it) {
        m__io->write_u4le((*it));
    }
    m__io->write_bytes(m_unknown2);
    if (m_device_names == nullptr) {
        throw std::runtime_error("/types/defaults/seq/9: repeated field is not set");
    }
    for (std::vector<std::string>::const_iterator it = m_device_names->begin(); it != m_device_names->end(); ++it) {
        m__io->write_bytes((*it));
    }
    if (m_track_names == nullptr) {
        throw std::runtime_error("/types/defaults/seq/10: repeated field is not set");
    }
    for (std::vector<std::string>::const_iterator it = m_track_names->begin(); it != m_track_names->end(); ++it) {
        m__io->write_bytes((*it));
    }
    if (m_devices == nullptr) {
        throw std::runtime_error("/types/defaults/seq/11: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_devices->begin(); it != m_devices->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_buses == nullptr) {
        throw std::runtime_error("/types/defaults/seq/12: repeated field is not set");
    }
    for (std::vector<bus_t>::const_iterator it = m_buses->begin(); it != m_buses->end(); ++it) {
        m__io->write_u1(static_cast<uint8_t>((*it)));
    }
    if (m_programs == nullptr) {
        throw std::runtime_error("/types/defaults/seq/13: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_programs->begin(); it != m_programs->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_track_velocities == nullptr) {
        throw std::runtime_error("/types/defaults/seq/14: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_track_velocities->begin(); it != m_track_velocities->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_track_statuses == nullptr) {
        throw std::runtime_error("/types/defaults/seq/15: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<track_status_t>>::const_iterator it = m_track_statuses->begin(); it != m_track_statuses->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/defaults/seq/15: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed16);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::defaults_t::_check() {
    if (m_sequence_name.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/defaults/seq/0: size mismatch");
    }
    if (m__unnamed1.size() != static_cast<std::string::size_type>(6)) {
        throw std::runtime_error("/types/defaults/seq/1: size mismatch");
    }
    if (m_unknown1 == nullptr) {
        throw std::runtime_error("/types/defaults/seq/7: repeated field is not set");
    }
    if (m_unknown1->size() != static_cast<std::size_t>(4)) {
        throw std::runtime_error("/types/defaults/seq/7: repeat-expr size mismatch");
    }
    for (std::vector<uint32_t>::const_iterator it = m_unknown1->begin(); it != m_unknown1->end(); ++it) {
    }
    if (m_unknown2.size() != static_cast<std::string::size_type>(74)) {
        throw std::runtime_error("/types/defaults/seq/8: size mismatch");
    }
    if (m_device_names == nullptr) {
        throw std::runtime_error("/types/defaults/seq/9: repeated field is not set");
    }
    if (m_device_names->size() != static_cast<std::size_t>(33)) {
        throw std::runtime_error("/types/defaults/seq/9: repeat-expr size mismatch");
    }
    for (std::vector<std::string>::const_iterator it = m_device_names->begin(); it != m_device_names->end(); ++it) {
        if ((*it).size() != static_cast<std::string::size_type>(8)) {
            throw std::runtime_error("/types/defaults/seq/9: size mismatch");
        }
    }
    if (m_track_names == nullptr) {
        throw std::runtime_error("/types/defaults/seq/10: repeated field is not set");
    }
    if (m_track_names->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/defaults/seq/10: repeat-expr size mismatch");
    }
    for (std::vector<std::string>::const_iterator it = m_track_names->begin(); it != m_track_names->end(); ++it) {
        if ((*it).size() != static_cast<std::string::size_type>(16)) {
            throw std::runtime_error("/types/defaults/seq/10: size mismatch");
        }
    }
    if (m_devices == nullptr) {
        throw std::runtime_error("/types/defaults/seq/11: repeated field is not set");
    }
    if (m_devices->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/defaults/seq/11: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_devices->begin(); it != m_devices->end(); ++it) {
    }
    if (m_buses == nullptr) {
        throw std::runtime_error("/types/defaults/seq/12: repeated field is not set");
    }
    if (m_buses->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/defaults/seq/12: repeat-expr size mismatch");
    }
    for (std::vector<bus_t>::const_iterator it = m_buses->begin(); it != m_buses->end(); ++it) {
    }
    if (m_programs == nullptr) {
        throw std::runtime_error("/types/defaults/seq/13: repeated field is not set");
    }
    if (m_programs->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/defaults/seq/13: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_programs->begin(); it != m_programs->end(); ++it) {
    }
    if (m_track_velocities == nullptr) {
        throw std::runtime_error("/types/defaults/seq/14: repeated field is not set");
    }
    if (m_track_velocities->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/defaults/seq/14: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_track_velocities->begin(); it != m_track_velocities->end(); ++it) {
    }
    if (m_track_statuses == nullptr) {
        throw std::runtime_error("/types/defaults/seq/15: repeated field is not set");
    }
    if (m_track_statuses->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/defaults/seq/15: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<track_status_t>>::const_iterator it = m_track_statuses->begin(); it != m_track_statuses->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/defaults/seq/15: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed16.size() != static_cast<std::string::size_type>(64)) {
        throw std::runtime_error("/types/defaults/seq/16: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::defaults_t::~defaults_t() {}

mpc2000xl_all_t::event_t::event_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    n_duration_bits_1 = true;
    n_track = true;
    n_duration_bits_2 = true;
    n_id = true;
    n_terminator_pad = true;
    n_terminator = true;
}

void mpc2000xl_all_t::event_t::_read() {
    m_tick = m__io->read_bits_int_le(20);
    n_duration_bits_1 = true;
    if (tick() < 1048575) {
        n_duration_bits_1 = false;
        m_duration_bits_1 = m__io->read_bits_int_le(4);
    }
    n_track = true;
    if (tick() < 1048575) {
        n_track = false;
        m_track = m__io->read_bits_int_le(6);
    }
    n_duration_bits_2 = true;
    if (tick() < 1048575) {
        n_duration_bits_2 = false;
        m_duration_bits_2 = m__io->read_bits_int_le(2);
    }
    n_id = true;
    if (tick() < 1048575) {
        n_id = false;
        m_id = m__io->read_u1();
    }
    n_terminator_pad = true;
    if (tick() >= 1048575) {
        n_terminator_pad = false;
        m_terminator_pad = m__io->read_bits_int_le(4);
    }
    if ( ((tick() < 1048575) && (id() <= 127)) ) {
        m_note_event = std::unique_ptr<note_event_t>(new note_event_t(m__io, this, m__root));
        m_note_event->_read();
    }
    if ( ((tick() < 1048575) && (id() == 224)) ) {
        m_pitch_bend = std::unique_ptr<pitch_bend_event_t>(new pitch_bend_event_t(m__io, this, m__root));
        m_pitch_bend->_read();
    }
    if ( ((tick() < 1048575) && (id() == 176)) ) {
        m_control_change = std::unique_ptr<control_change_event_t>(new control_change_event_t(m__io, this, m__root));
        m_control_change->_read();
    }
    if ( ((tick() < 1048575) && (id() == 192)) ) {
        m_program_change = std::unique_ptr<program_change_event_t>(new program_change_event_t(m__io, this, m__root));
        m_program_change->_read();
    }
    if ( ((tick() < 1048575) && (id() == 208)) ) {
        m_ch_pressure = std::unique_ptr<ch_pressure_event_t>(new ch_pressure_event_t(m__io, this, m__root));
        m_ch_pressure->_read();
    }
    if ( ((tick() < 1048575) && (id() == 160)) ) {
        m_poly_pressure = std::unique_ptr<poly_pressure_event_t>(new poly_pressure_event_t(m__io, this, m__root));
        m_poly_pressure->_read();
    }
    if ( ((tick() < 1048575) && (id() == 240)) ) {
        m_exclusive = std::unique_ptr<exclusive_event_t>(new exclusive_event_t(m__io, this, m__root));
        m_exclusive->_read();
    }
    n_terminator = true;
    if (tick() >= 1048575) {
        n_terminator = false;
        m_terminator = m__io->read_bytes(5);
    }
    m__dirty = false;
}

void mpc2000xl_all_t::event_t::_fetch_instances() {
    n_duration_bits_1 = true;
    if (tick() < 1048575) {
        n_duration_bits_1 = false;
    }
    n_track = true;
    if (tick() < 1048575) {
        n_track = false;
    }
    n_duration_bits_2 = true;
    if (tick() < 1048575) {
        n_duration_bits_2 = false;
    }
    n_id = true;
    if (tick() < 1048575) {
        n_id = false;
    }
    n_terminator_pad = true;
    if (tick() >= 1048575) {
        n_terminator_pad = false;
    }
    if ( ((tick() < 1048575) && (id() <= 127)) ) {
        m_note_event.get()->_fetch_instances();
    }
    if ( ((tick() < 1048575) && (id() == 224)) ) {
        m_pitch_bend.get()->_fetch_instances();
    }
    if ( ((tick() < 1048575) && (id() == 176)) ) {
        m_control_change.get()->_fetch_instances();
    }
    if ( ((tick() < 1048575) && (id() == 192)) ) {
        m_program_change.get()->_fetch_instances();
    }
    if ( ((tick() < 1048575) && (id() == 208)) ) {
        m_ch_pressure.get()->_fetch_instances();
    }
    if ( ((tick() < 1048575) && (id() == 160)) ) {
        m_poly_pressure.get()->_fetch_instances();
    }
    if ( ((tick() < 1048575) && (id() == 240)) ) {
        m_exclusive.get()->_fetch_instances();
    }
    n_terminator = true;
    if (tick() >= 1048575) {
        n_terminator = false;
    }
}

void mpc2000xl_all_t::event_t::_write() {
    m__io->write_bits_int_le(20, m_tick);
    if (tick() < 1048575) {
        m__io->write_bits_int_le(4, m_duration_bits_1);
    }
    if (tick() < 1048575) {
        m__io->write_bits_int_le(6, m_track);
    }
    if (tick() < 1048575) {
        m__io->write_bits_int_le(2, m_duration_bits_2);
    }
    if (tick() < 1048575) {
        m__io->write_u1(m_id);
    }
    if (tick() >= 1048575) {
        m__io->write_bits_int_le(4, m_terminator_pad);
    }
    if ( ((tick() < 1048575) && (id() <= 127)) ) {
        if (m_note_event.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/6: nested object is not set");
        }
        m_note_event.get()->_set_io(m__io);
        m_note_event.get()->_write();
    }
    if ( ((tick() < 1048575) && (id() == 224)) ) {
        if (m_pitch_bend.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/7: nested object is not set");
        }
        m_pitch_bend.get()->_set_io(m__io);
        m_pitch_bend.get()->_write();
    }
    if ( ((tick() < 1048575) && (id() == 176)) ) {
        if (m_control_change.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/8: nested object is not set");
        }
        m_control_change.get()->_set_io(m__io);
        m_control_change.get()->_write();
    }
    if ( ((tick() < 1048575) && (id() == 192)) ) {
        if (m_program_change.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/9: nested object is not set");
        }
        m_program_change.get()->_set_io(m__io);
        m_program_change.get()->_write();
    }
    if ( ((tick() < 1048575) && (id() == 208)) ) {
        if (m_ch_pressure.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/10: nested object is not set");
        }
        m_ch_pressure.get()->_set_io(m__io);
        m_ch_pressure.get()->_write();
    }
    if ( ((tick() < 1048575) && (id() == 160)) ) {
        if (m_poly_pressure.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/11: nested object is not set");
        }
        m_poly_pressure.get()->_set_io(m__io);
        m_poly_pressure.get()->_write();
    }
    if ( ((tick() < 1048575) && (id() == 240)) ) {
        if (m_exclusive.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/12: nested object is not set");
        }
        m_exclusive.get()->_set_io(m__io);
        m_exclusive.get()->_write();
    }
    if (tick() >= 1048575) {
        m__io->write_bytes(m_terminator);
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::event_t::_check() {
    if (tick() < 1048575) {
        if (!(!n_duration_bits_1)) {
            throw std::runtime_error("/types/event/seq/1: conditional field is not set");
        }
    } else {
        if (!n_duration_bits_1) {
            throw std::runtime_error("/types/event/seq/1: conditional field should be absent");
        }
    }
    if (tick() < 1048575) {
        if (!(!n_track)) {
            throw std::runtime_error("/types/event/seq/2: conditional field is not set");
        }
    } else {
        if (!n_track) {
            throw std::runtime_error("/types/event/seq/2: conditional field should be absent");
        }
    }
    if (tick() < 1048575) {
        if (!(!n_duration_bits_2)) {
            throw std::runtime_error("/types/event/seq/3: conditional field is not set");
        }
    } else {
        if (!n_duration_bits_2) {
            throw std::runtime_error("/types/event/seq/3: conditional field should be absent");
        }
    }
    if (tick() < 1048575) {
        if (!(!n_id)) {
            throw std::runtime_error("/types/event/seq/4: conditional field is not set");
        }
    } else {
        if (!n_id) {
            throw std::runtime_error("/types/event/seq/4: conditional field should be absent");
        }
    }
    if (tick() >= 1048575) {
        if (!(!n_terminator_pad)) {
            throw std::runtime_error("/types/event/seq/5: conditional field is not set");
        }
    } else {
        if (!n_terminator_pad) {
            throw std::runtime_error("/types/event/seq/5: conditional field should be absent");
        }
    }
    if ( ((tick() < 1048575) && (id() <= 127)) ) {
        if (!(m_note_event.get() != nullptr)) {
            throw std::runtime_error("/types/event/seq/6: conditional field is not set");
        }
        if (m_note_event.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/6: nested object is not set");
        }
        m_note_event.get()->_set_io(m__io);
        m_note_event.get()->_check();
    } else {
        if (m_note_event.get() != nullptr) {
            throw std::runtime_error("/types/event/seq/6: conditional field should be absent");
        }
    }
    if ( ((tick() < 1048575) && (id() == 224)) ) {
        if (!(m_pitch_bend.get() != nullptr)) {
            throw std::runtime_error("/types/event/seq/7: conditional field is not set");
        }
        if (m_pitch_bend.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/7: nested object is not set");
        }
        m_pitch_bend.get()->_set_io(m__io);
        m_pitch_bend.get()->_check();
    } else {
        if (m_pitch_bend.get() != nullptr) {
            throw std::runtime_error("/types/event/seq/7: conditional field should be absent");
        }
    }
    if ( ((tick() < 1048575) && (id() == 176)) ) {
        if (!(m_control_change.get() != nullptr)) {
            throw std::runtime_error("/types/event/seq/8: conditional field is not set");
        }
        if (m_control_change.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/8: nested object is not set");
        }
        m_control_change.get()->_set_io(m__io);
        m_control_change.get()->_check();
    } else {
        if (m_control_change.get() != nullptr) {
            throw std::runtime_error("/types/event/seq/8: conditional field should be absent");
        }
    }
    if ( ((tick() < 1048575) && (id() == 192)) ) {
        if (!(m_program_change.get() != nullptr)) {
            throw std::runtime_error("/types/event/seq/9: conditional field is not set");
        }
        if (m_program_change.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/9: nested object is not set");
        }
        m_program_change.get()->_set_io(m__io);
        m_program_change.get()->_check();
    } else {
        if (m_program_change.get() != nullptr) {
            throw std::runtime_error("/types/event/seq/9: conditional field should be absent");
        }
    }
    if ( ((tick() < 1048575) && (id() == 208)) ) {
        if (!(m_ch_pressure.get() != nullptr)) {
            throw std::runtime_error("/types/event/seq/10: conditional field is not set");
        }
        if (m_ch_pressure.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/10: nested object is not set");
        }
        m_ch_pressure.get()->_set_io(m__io);
        m_ch_pressure.get()->_check();
    } else {
        if (m_ch_pressure.get() != nullptr) {
            throw std::runtime_error("/types/event/seq/10: conditional field should be absent");
        }
    }
    if ( ((tick() < 1048575) && (id() == 160)) ) {
        if (!(m_poly_pressure.get() != nullptr)) {
            throw std::runtime_error("/types/event/seq/11: conditional field is not set");
        }
        if (m_poly_pressure.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/11: nested object is not set");
        }
        m_poly_pressure.get()->_set_io(m__io);
        m_poly_pressure.get()->_check();
    } else {
        if (m_poly_pressure.get() != nullptr) {
            throw std::runtime_error("/types/event/seq/11: conditional field should be absent");
        }
    }
    if ( ((tick() < 1048575) && (id() == 240)) ) {
        if (!(m_exclusive.get() != nullptr)) {
            throw std::runtime_error("/types/event/seq/12: conditional field is not set");
        }
        if (m_exclusive.get() == nullptr) {
            throw std::runtime_error("/types/event/seq/12: nested object is not set");
        }
        m_exclusive.get()->_set_io(m__io);
        m_exclusive.get()->_check();
    } else {
        if (m_exclusive.get() != nullptr) {
            throw std::runtime_error("/types/event/seq/12: conditional field should be absent");
        }
    }
    if (tick() >= 1048575) {
        if (!(!n_terminator)) {
            throw std::runtime_error("/types/event/seq/13: conditional field is not set");
        }
        if (m_terminator.size() != static_cast<std::string::size_type>(5)) {
            throw std::runtime_error("/types/event/seq/13: size mismatch");
        }
    } else {
        if (!n_terminator) {
            throw std::runtime_error("/types/event/seq/13: conditional field should be absent");
        }
    }
    m__dirty = false;
}

mpc2000xl_all_t::event_t::~event_t() {}

mpc2000xl_all_t::exclusive_event_t::exclusive_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::exclusive_event_t::_read() {
    m__unnamed0 = m__io->read_bytes(3);
    m_bytes = m__io->read_bytes(2);
    if (bytes() == std::string("\xF0\x47", 2)) {
        m_mixer = std::unique_ptr<mixer_event_t>(new mixer_event_t(m__io, this, m__root));
        m_mixer->_read();
    }
    m__unnamed3 = m__io->read_bytes(14);
    m__dirty = false;
}

void mpc2000xl_all_t::exclusive_event_t::_fetch_instances() {
    if (bytes() == std::string("\xF0\x47", 2)) {
        m_mixer.get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::exclusive_event_t::_write() {
    m__io->write_bytes(m__unnamed0);
    m__io->write_bytes(m_bytes);
    if (bytes() == std::string("\xF0\x47", 2)) {
        if (m_mixer.get() == nullptr) {
            throw std::runtime_error("/types/exclusive_event/seq/2: nested object is not set");
        }
        m_mixer.get()->_set_io(m__io);
        m_mixer.get()->_write();
    }
    m__io->write_bytes(m__unnamed3);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::exclusive_event_t::_check() {
    if (m__unnamed0.size() != static_cast<std::string::size_type>(3)) {
        throw std::runtime_error("/types/exclusive_event/seq/0: size mismatch");
    }
    if (m_bytes.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/types/exclusive_event/seq/1: size mismatch");
    }
    if (bytes() == std::string("\xF0\x47", 2)) {
        if (!(m_mixer.get() != nullptr)) {
            throw std::runtime_error("/types/exclusive_event/seq/2: conditional field is not set");
        }
        if (m_mixer.get() == nullptr) {
            throw std::runtime_error("/types/exclusive_event/seq/2: nested object is not set");
        }
        m_mixer.get()->_set_io(m__io);
        m_mixer.get()->_check();
    } else {
        if (m_mixer.get() != nullptr) {
            throw std::runtime_error("/types/exclusive_event/seq/2: conditional field should be absent");
        }
    }
    if (m__unnamed3.size() != static_cast<std::string::size_type>(14)) {
        throw std::runtime_error("/types/exclusive_event/seq/3: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::exclusive_event_t::~exclusive_event_t() {}

mpc2000xl_all_t::location_t::location_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_display_value = false;
}

void mpc2000xl_all_t::location_t::_read() {
    m_bar = m__io->read_u2le();
    m_beat = m__io->read_u1();
    m_clock = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_all_t::location_t::_fetch_instances() {
}

void mpc2000xl_all_t::location_t::_write() {
    m__io->write_u2le(m_bar);
    m__io->write_u1(m_beat);
    m__io->write_u1(m_clock);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::location_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::location_t::~location_t() {}

std::string mpc2000xl_all_t::location_t::display_value() {
    if (f_display_value)
        return m_display_value;
    f_display_value = true;
    m_display_value = (((((bar() > 98) ? (kaitai::kstream::to_string(bar() + 1)) : (((bar() > 8) ? (std::string("0") + kaitai::kstream::to_string(bar() + 1)) : (std::string("00") + kaitai::kstream::to_string(bar() + 1))))) + std::string(".")) + ((beat() > 8) ? (kaitai::kstream::to_string(beat() + 1)) : (std::string("0") + kaitai::kstream::to_string(beat() + 1)))) + std::string(".")) + ((clock() > 9) ? (kaitai::kstream::to_string(clock())) : (std::string("0") + kaitai::kstream::to_string(clock())));
    return m_display_value;
}

mpc2000xl_all_t::midi_input_t::midi_input_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::midi_input_t::_read() {
    m_receive_channel = static_cast<mpc2000xl_all_t::midi_input_receive_channel_t>(m__io->read_u1());
    m_sustain_pedal_to_duration = m__io->read_bits_int_le(1);
    m__unnamed2 = m__io->read_bits_int_le(7);
    m_filter_enabled = m__io->read_bits_int_le(1);
    m__unnamed4 = m__io->read_bits_int_le(7);
    m_filter_type = static_cast<mpc2000xl_all_t::midi_input_filter_type_t>(m__io->read_u1());
    m_multi_rec_enabled = m__io->read_bits_int_le(1);
    m__unnamed7 = m__io->read_bits_int_le(7);
    m_multi_rec_destination_tracks = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_multi_rec_destination_tracks = 34;
    for (int i = 0; i < l_multi_rec_destination_tracks; i++) {
        m_multi_rec_destination_tracks->push_back(std::move(m__io->read_u1()));
    }
    m_note_pass_enabled = m__io->read_bits_int_le(1);
    m__unnamed10 = m__io->read_bits_int_le(7);
    m_pitch_bend_pass_enabled = m__io->read_bits_int_le(1);
    m__unnamed12 = m__io->read_bits_int_le(7);
    m_pgm_change_pass_enabled = m__io->read_bits_int_le(1);
    m__unnamed14 = m__io->read_bits_int_le(7);
    m_ch_pressure_pass_enabled = m__io->read_bits_int_le(1);
    m__unnamed16 = m__io->read_bits_int_le(7);
    m_poly_pressure_pass_enabled = m__io->read_bits_int_le(1);
    m__unnamed18 = m__io->read_bits_int_le(7);
    m_exclusive_pass_enabled = m__io->read_bits_int_le(1);
    m__unnamed20 = m__io->read_bits_int_le(7);
    m_cc_pass_enabled = std::unique_ptr<std::vector<bool>>(new std::vector<bool>());
    const int l_cc_pass_enabled = 128;
    for (int i = 0; i < l_cc_pass_enabled; i++) {
        m_cc_pass_enabled->push_back(std::move(m__io->read_bits_int_le(1)));
    }
    m__dirty = false;
}

void mpc2000xl_all_t::midi_input_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_multi_rec_destination_tracks->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_cc_pass_enabled->size(); ++i) {
    }
}

void mpc2000xl_all_t::midi_input_t::_write() {
    m__io->write_u1(static_cast<uint8_t>(m_receive_channel));
    m__io->write_bits_int_le(1, ((m_sustain_pedal_to_duration) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed2);
    m__io->write_bits_int_le(1, ((m_filter_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed4);
    m__io->write_u1(static_cast<uint8_t>(m_filter_type));
    m__io->write_bits_int_le(1, ((m_multi_rec_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed7);
    if (m_multi_rec_destination_tracks == nullptr) {
        throw std::runtime_error("/types/midi_input/seq/8: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_multi_rec_destination_tracks->begin(); it != m_multi_rec_destination_tracks->end(); ++it) {
        m__io->write_u1((*it));
    }
    m__io->write_bits_int_le(1, ((m_note_pass_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed10);
    m__io->write_bits_int_le(1, ((m_pitch_bend_pass_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed12);
    m__io->write_bits_int_le(1, ((m_pgm_change_pass_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed14);
    m__io->write_bits_int_le(1, ((m_ch_pressure_pass_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed16);
    m__io->write_bits_int_le(1, ((m_poly_pressure_pass_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed18);
    m__io->write_bits_int_le(1, ((m_exclusive_pass_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed20);
    if (m_cc_pass_enabled == nullptr) {
        throw std::runtime_error("/types/midi_input/seq/21: repeated field is not set");
    }
    for (std::vector<bool>::const_iterator it = m_cc_pass_enabled->begin(); it != m_cc_pass_enabled->end(); ++it) {
        m__io->write_bits_int_le(1, (((*it)) ? 1 : 0));
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::midi_input_t::_check() {
    if (m_multi_rec_destination_tracks == nullptr) {
        throw std::runtime_error("/types/midi_input/seq/8: repeated field is not set");
    }
    if (m_multi_rec_destination_tracks->size() != static_cast<std::size_t>(34)) {
        throw std::runtime_error("/types/midi_input/seq/8: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_multi_rec_destination_tracks->begin(); it != m_multi_rec_destination_tracks->end(); ++it) {
    }
    if (m_cc_pass_enabled == nullptr) {
        throw std::runtime_error("/types/midi_input/seq/21: repeated field is not set");
    }
    if (m_cc_pass_enabled->size() != static_cast<std::size_t>(128)) {
        throw std::runtime_error("/types/midi_input/seq/21: repeat-expr size mismatch");
    }
    for (std::vector<bool>::const_iterator it = m_cc_pass_enabled->begin(); it != m_cc_pass_enabled->end(); ++it) {
    }
    m__dirty = false;
}

mpc2000xl_all_t::midi_input_t::~midi_input_t() {}

mpc2000xl_all_t::midi_output_t::midi_output_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::midi_output_t::_read() {
    m_soft_thru_mode = static_cast<mpc2000xl_all_t::soft_thru_mode_t>(m__io->read_u1());
    m__dirty = false;
}

void mpc2000xl_all_t::midi_output_t::_fetch_instances() {
}

void mpc2000xl_all_t::midi_output_t::_write() {
    m__io->write_u1(static_cast<uint8_t>(m_soft_thru_mode));
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::midi_output_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::midi_output_t::~midi_output_t() {}

mpc2000xl_all_t::midi_switch_t::midi_switch_t(kaitai::kstream* p__io, mpc2000xl_all_t::misc_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::midi_switch_t::_read() {
    m_controller = m__io->read_u1();
    m_function = static_cast<mpc2000xl_all_t::midi_switch_function_t>(m__io->read_u1());
    m__dirty = false;
}

void mpc2000xl_all_t::midi_switch_t::_fetch_instances() {
}

void mpc2000xl_all_t::midi_switch_t::_write() {
    m__io->write_u1(m_controller);
    m__io->write_u1(static_cast<uint8_t>(m_function));
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::midi_switch_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::midi_switch_t::~midi_switch_t() {}

mpc2000xl_all_t::midi_sync_t::midi_sync_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::midi_sync_t::_read() {
    m_in_mode = static_cast<mpc2000xl_all_t::midi_sync_mode_t>(m__io->read_u1());
    m_out_mode = static_cast<mpc2000xl_all_t::midi_sync_mode_t>(m__io->read_u1());
    m_shift_early = m__io->read_u1();
    if (!(m_shift_early >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_shift_early, m__io, std::string("/types/midi_sync/seq/2"));
    }
    if (!(m_shift_early <= 20)) {
        throw kaitai::validation_greater_than_error<uint8_t>(20, m_shift_early, m__io, std::string("/types/midi_sync/seq/2"));
    }
    m_send_mmc_enabled = m__io->read_bits_int_le(1);
    m__unnamed4 = m__io->read_bits_int_le(7);
    m_frame_rate = static_cast<mpc2000xl_all_t::frame_rate_t>(m__io->read_u1());
    m_input = m__io->read_u1();
    m_output = static_cast<mpc2000xl_all_t::midi_sync_output_t>(m__io->read_u1());
    m__dirty = false;
}

void mpc2000xl_all_t::midi_sync_t::_fetch_instances() {
}

void mpc2000xl_all_t::midi_sync_t::_write() {
    m__io->write_u1(static_cast<uint8_t>(m_in_mode));
    m__io->write_u1(static_cast<uint8_t>(m_out_mode));
    m__io->write_u1(m_shift_early);
    m__io->write_bits_int_le(1, ((m_send_mmc_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed4);
    m__io->write_u1(static_cast<uint8_t>(m_frame_rate));
    m__io->write_u1(m_input);
    m__io->write_u1(static_cast<uint8_t>(m_output));
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::midi_sync_t::_check() {
    if (!(m_shift_early >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_shift_early, m__io, std::string("/types/midi_sync/seq/2"));
    }
    if (!(m_shift_early <= 20)) {
        throw kaitai::validation_greater_than_error<uint8_t>(20, m_shift_early, m__io, std::string("/types/midi_sync/seq/2"));
    }
    m__dirty = false;
}

mpc2000xl_all_t::midi_sync_t::~midi_sync_t() {}

mpc2000xl_all_t::misc_t::misc_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::misc_t::_read() {
    m_tap_averaging = static_cast<mpc2000xl_all_t::tap_averaging_t>(m__io->read_u1());
    m_midi_sync_in_receive_mmc_enabled = m__io->read_bits_int_le(1);
    m_midi_switch = std::unique_ptr<std::vector<std::unique_ptr<midi_switch_t>>>(new std::vector<std::unique_ptr<midi_switch_t>>());
    const int l_midi_switch = 4;
    for (int i = 0; i < l_midi_switch; i++) {
        std::unique_ptr<midi_switch_t> _t_midi_switch = std::unique_ptr<midi_switch_t>(new midi_switch_t(m__io, this, m__root));
        try {
            _t_midi_switch->_read();
        } catch(...) {
            m_midi_switch->push_back(std::move(_t_midi_switch));
            throw;
        }
        m_midi_switch->push_back(std::move(_t_midi_switch));
    }
    m__dirty = false;
}

void mpc2000xl_all_t::misc_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_midi_switch->size(); ++i) {
        m_midi_switch->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::misc_t::_write() {
    m__io->write_u1(static_cast<uint8_t>(m_tap_averaging));
    m__io->write_bits_int_le(1, ((m_midi_sync_in_receive_mmc_enabled) ? 1 : 0));
    if (m_midi_switch == nullptr) {
        throw std::runtime_error("/types/misc/seq/2: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<midi_switch_t>>::const_iterator it = m_midi_switch->begin(); it != m_midi_switch->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc/seq/2: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::misc_t::_check() {
    if (m_midi_switch == nullptr) {
        throw std::runtime_error("/types/misc/seq/2: repeated field is not set");
    }
    if (m_midi_switch->size() != static_cast<std::size_t>(4)) {
        throw std::runtime_error("/types/misc/seq/2: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<midi_switch_t>>::const_iterator it = m_midi_switch->begin(); it != m_midi_switch->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc/seq/2: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

mpc2000xl_all_t::misc_t::~misc_t() {}

mpc2000xl_all_t::mixer_event_t::mixer_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::exclusive_event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::mixer_event_t::_read() {
    m__unnamed0 = m__io->read_bytes(3);
    m_param = static_cast<mpc2000xl_all_t::mixer_event_param_t>(m__io->read_u1());
    m_pad_index = m__io->read_u1();
    m_value = m__io->read_u1();
    if (!(m_value >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_value, m__io, std::string("/types/mixer_event/seq/3"));
    }
    if (!(m_value <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_value, m__io, std::string("/types/mixer_event/seq/3"));
    }
    m__unnamed4 = m__io->read_bytes(2);
    m__dirty = false;
}

void mpc2000xl_all_t::mixer_event_t::_fetch_instances() {
}

void mpc2000xl_all_t::mixer_event_t::_write() {
    m__io->write_bytes(m__unnamed0);
    m__io->write_u1(static_cast<uint8_t>(m_param));
    m__io->write_u1(m_pad_index);
    m__io->write_u1(m_value);
    m__io->write_bytes(m__unnamed4);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::mixer_event_t::_check() {
    if (m__unnamed0.size() != static_cast<std::string::size_type>(3)) {
        throw std::runtime_error("/types/mixer_event/seq/0: size mismatch");
    }
    if (!(m_value >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_value, m__io, std::string("/types/mixer_event/seq/3"));
    }
    if (!(m_value <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_value, m__io, std::string("/types/mixer_event/seq/3"));
    }
    if (m__unnamed4.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/types/mixer_event/seq/4: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::mixer_event_t::~mixer_event_t() {}
const std::set<mpc2000xl_all_t::note_event_t::note_variation_type_t> mpc2000xl_all_t::note_event_t::_values_note_variation_type_t{
    mpc2000xl_all_t::note_event_t::NOTE_VARIATION_TYPE_TUNE,
    mpc2000xl_all_t::note_event_t::NOTE_VARIATION_TYPE_DECAY,
    mpc2000xl_all_t::note_event_t::NOTE_VARIATION_TYPE_ATTACK,
    mpc2000xl_all_t::note_event_t::NOTE_VARIATION_TYPE_FILTER,
};
bool mpc2000xl_all_t::note_event_t::_is_defined_note_variation_type_t(mpc2000xl_all_t::note_event_t::note_variation_type_t v) {
    return mpc2000xl_all_t::note_event_t::_values_note_variation_type_t.find(v) != mpc2000xl_all_t::note_event_t::_values_note_variation_type_t.end();
}

mpc2000xl_all_t::note_event_t::note_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_duration = false;
    f_note = false;
    f_variation_type = false;
}

void mpc2000xl_all_t::note_event_t::_read() {
    m_duration_bits_3 = m__io->read_u1();
    m_velocity = m__io->read_bits_int_le(7);
    m_variation_type_bit_1 = m__io->read_bits_int_le(1);
    m_variation_value = m__io->read_bits_int_le(7);
    m_variation_type_bit_2 = m__io->read_bits_int_le(1);
    m__dirty = false;
}

void mpc2000xl_all_t::note_event_t::_fetch_instances() {
}

void mpc2000xl_all_t::note_event_t::_write() {
    m__io->write_u1(m_duration_bits_3);
    m__io->write_bits_int_le(7, m_velocity);
    m__io->write_bits_int_le(1, ((m_variation_type_bit_1) ? 1 : 0));
    m__io->write_bits_int_le(7, m_variation_value);
    m__io->write_bits_int_le(1, ((m_variation_type_bit_2) ? 1 : 0));
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::note_event_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::note_event_t::~note_event_t() {}

int32_t mpc2000xl_all_t::note_event_t::duration() {
    if (f_duration)
        return m_duration;
    f_duration = true;
    m_duration = ((static_cast<mpc2000xl_all_t::event_t*>(_parent())->duration_bits_1() << 10) + (static_cast<mpc2000xl_all_t::event_t*>(_parent())->duration_bits_2() << 8)) + duration_bits_3();
    return m_duration;
}

uint8_t mpc2000xl_all_t::note_event_t::note() {
    if (f_note)
        return m_note;
    f_note = true;
    m_note = static_cast<mpc2000xl_all_t::event_t*>(_parent())->id();
    return m_note;
}

mpc2000xl_all_t::note_event_t::note_variation_type_t mpc2000xl_all_t::note_event_t::variation_type() {
    if (f_variation_type)
        return m_variation_type;
    f_variation_type = true;
    m_variation_type = static_cast<mpc2000xl_all_t::note_event_t::note_variation_type_t>(static_cast<uint8_t>(variation_type_bit_1()) << 1 | static_cast<uint8_t>(variation_type_bit_2()));
    return m_variation_type;
}

mpc2000xl_all_t::pitch_bend_event_t::pitch_bend_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_corrected_amount = false;
}

void mpc2000xl_all_t::pitch_bend_event_t::_read() {
    m_amount_bits_1 = m__io->read_bits_int_le(8);
    m_amount_bits_2 = m__io->read_bits_int_le(8);
    m__unnamed2 = m__io->read_bytes(1);
    m__dirty = false;
}

void mpc2000xl_all_t::pitch_bend_event_t::_fetch_instances() {
}

void mpc2000xl_all_t::pitch_bend_event_t::_write() {
    m__io->write_bits_int_le(8, m_amount_bits_1);
    m__io->write_bits_int_le(8, m_amount_bits_2);
    m__io->write_bytes(m__unnamed2);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::pitch_bend_event_t::_check() {
    if (m__unnamed2.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/pitch_bend_event/seq/2: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::pitch_bend_event_t::~pitch_bend_event_t() {}

int32_t mpc2000xl_all_t::pitch_bend_event_t::corrected_amount() {
    if (f_corrected_amount)
        return m_corrected_amount;
    f_corrected_amount = true;
    m_corrected_amount = (amount_bits_1() + (amount_bits_2() << 7)) - 8192;
    return m_corrected_amount;
}

mpc2000xl_all_t::poly_pressure_event_t::poly_pressure_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::poly_pressure_event_t::_read() {
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
    m__unnamed2 = m__io->read_bytes(1);
    m__dirty = false;
}

void mpc2000xl_all_t::poly_pressure_event_t::_fetch_instances() {
}

void mpc2000xl_all_t::poly_pressure_event_t::_write() {
    m__io->write_u1(m_note);
    m__io->write_u1(m_pressure);
    m__io->write_bytes(m__unnamed2);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::poly_pressure_event_t::_check() {
    if (!(m_note >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_note, m__io, std::string("/types/poly_pressure_event/seq/0"));
    }
    if (!(m_note <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_note, m__io, std::string("/types/poly_pressure_event/seq/0"));
    }
    if (!(m_pressure >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_pressure, m__io, std::string("/types/poly_pressure_event/seq/1"));
    }
    if (!(m_pressure <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_pressure, m__io, std::string("/types/poly_pressure_event/seq/1"));
    }
    if (m__unnamed2.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/poly_pressure_event/seq/2: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::poly_pressure_event_t::~poly_pressure_event_t() {}

mpc2000xl_all_t::program_change_event_t::program_change_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::program_change_event_t::_read() {
    m_program = m__io->read_u1();
    if (!(m_program >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_program, m__io, std::string("/types/program_change_event/seq/0"));
    }
    if (!(m_program <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_program, m__io, std::string("/types/program_change_event/seq/0"));
    }
    m__unnamed1 = m__io->read_bytes(2);
    m__dirty = false;
}

void mpc2000xl_all_t::program_change_event_t::_fetch_instances() {
}

void mpc2000xl_all_t::program_change_event_t::_write() {
    m__io->write_u1(m_program);
    m__io->write_bytes(m__unnamed1);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::program_change_event_t::_check() {
    if (!(m_program >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_program, m__io, std::string("/types/program_change_event/seq/0"));
    }
    if (!(m_program <= 127)) {
        throw kaitai::validation_greater_than_error<uint8_t>(127, m_program, m__io, std::string("/types/program_change_event/seq/0"));
    }
    if (m__unnamed1.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/types/program_change_event/seq/1: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::program_change_event_t::~program_change_event_t() {}

mpc2000xl_all_t::sequence_t::sequence_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    n_name_part_2 = true;
    f_name = false;
}

void mpc2000xl_all_t::sequence_t::_read() {
    m_name_part_1 = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(kaitai::kstream::bytes_strip_right(m__io->read_bytes(8), static_cast<char>(255)), static_cast<char>(255), false), "ASCII");
    n_name_part_2 = true;
    if (name_part_1() != std::string("")) {
        n_name_part_2 = false;
        m_name_part_2 = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(kaitai::kstream::bytes_strip_right(m__io->read_bytes(8), static_cast<char>(255)), static_cast<char>(255), false), "ASCII");
    }
    if (name_part_1() != std::string("")) {
        m_body = std::unique_ptr<sequence_body_t>(new sequence_body_t(m__io, this, m__root));
        m_body->_read();
    }
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_t::_fetch_instances() {
    n_name_part_2 = true;
    if (name_part_1() != std::string("")) {
        n_name_part_2 = false;
    }
    if (name_part_1() != std::string("")) {
        m_body.get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::sequence_t::_write() {
    std::string _bufm_name_part_1 = m_name_part_1;
    if (_bufm_name_part_1.size() < static_cast<std::string::size_type>(8)) {
        _bufm_name_part_1 += std::string(1, static_cast<char>(255));
    }
    if (_bufm_name_part_1.size() < static_cast<std::string::size_type>(8)) {
        _bufm_name_part_1.append(static_cast<std::string::size_type>(8) - _bufm_name_part_1.size(), static_cast<char>(255));
    }
    m__io->write_bytes(_bufm_name_part_1);
    if (name_part_1() != std::string("")) {
        std::string _bufm_name_part_2 = m_name_part_2;
        if (_bufm_name_part_2.size() < static_cast<std::string::size_type>(8)) {
            _bufm_name_part_2 += std::string(1, static_cast<char>(255));
        }
        if (_bufm_name_part_2.size() < static_cast<std::string::size_type>(8)) {
            _bufm_name_part_2.append(static_cast<std::string::size_type>(8) - _bufm_name_part_2.size(), static_cast<char>(255));
        }
        m__io->write_bytes(_bufm_name_part_2);
    }
    if (name_part_1() != std::string("")) {
        if (m_body.get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/2: nested object is not set");
        }
        m_body.get()->_set_io(m__io);
        m_body.get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_t::_check() {
    if (m_name_part_1.size() > static_cast<std::string::size_type>(8)) {
        throw std::runtime_error("/types/sequence/seq/0: size mismatch");
    }
    if (m_name_part_1.find(static_cast<char>(255)) != std::string::npos) {
        throw std::runtime_error("/types/sequence/seq/0: terminator must not appear in value");
    }
    if (name_part_1() != std::string("")) {
        if (!(!n_name_part_2)) {
            throw std::runtime_error("/types/sequence/seq/1: conditional field is not set");
        }
        if (m_name_part_2.size() > static_cast<std::string::size_type>(8)) {
            throw std::runtime_error("/types/sequence/seq/1: size mismatch");
        }
        if (m_name_part_2.find(static_cast<char>(255)) != std::string::npos) {
            throw std::runtime_error("/types/sequence/seq/1: terminator must not appear in value");
        }
    } else {
        if (!n_name_part_2) {
            throw std::runtime_error("/types/sequence/seq/1: conditional field should be absent");
        }
    }
    if (name_part_1() != std::string("")) {
        if (!(m_body.get() != nullptr)) {
            throw std::runtime_error("/types/sequence/seq/2: conditional field is not set");
        }
        if (m_body.get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/2: nested object is not set");
        }
        m_body.get()->_set_io(m__io);
        m_body.get()->_check();
    } else {
        if (m_body.get() != nullptr) {
            throw std::runtime_error("/types/sequence/seq/2: conditional field should be absent");
        }
    }
    m__dirty = false;
}

mpc2000xl_all_t::sequence_t::~sequence_t() {}

std::string mpc2000xl_all_t::sequence_t::name() {
    if (f_name)
        return m_name;
    f_name = true;
    n_name = true;
    if ( ((name_part_1() != std::string("")) && (name_part_2() != std::string(""))) ) {
        n_name = false;
        m_name = name_part_1() + name_part_2();
    }
    return m_name;
}

mpc2000xl_all_t::sequence_body_t::sequence_body_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_body_t::_read() {
    m_is_used = static_cast<mpc2000xl_all_t::sequence_is_used_t>(m__io->read_u2le());
    m_index = m__io->read_u1();
    m__unnamed2 = m__io->read_bytes(7);
    m_bar_count = m__io->read_u2le();
    m_last_tick = m__io->read_u4le();
    m__unnamed5 = m__io->read_bytes(16);
    m_loop_start_bar_index = m__io->read_u2le();
    m_loop_end_bar_index = m__io->read_u2le();
    m_loop_enabled = m__io->read_bits_int_le(1);
    m__unnamed9 = m__io->read_bits_int_le(7);
    m_start_time = std::unique_ptr<start_time_t>(new start_time_t(m__io, this, m__root));
    m_start_time->_read();
    m__unnamed11 = m__io->read_bytes(6);
    m_last_tick2 = m__io->read_u4le();
    m__unnamed13 = m__io->read_bytes(52);
    m_device_names = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    const int l_device_names = 33;
    for (int i = 0; i < l_device_names; i++) {
        m_device_names->push_back(std::move(kaitai::kstream::bytes_to_str(m__io->read_bytes(8), "ASCII")));
    }
    m_tracks = std::unique_ptr<tracks_t>(new tracks_t(m__io, this, m__root));
    m_tracks->_read();
    m__unnamed16 = m__io->read_bytes(3587);
    m_bars = std::unique_ptr<std::vector<std::unique_ptr<bar_t>>>(new std::vector<std::unique_ptr<bar_t>>());
    const int l_bars = bar_count();
    for (int i = 0; i < l_bars; i++) {
        std::unique_ptr<bar_t> _t_bars = std::unique_ptr<bar_t>(new bar_t(i, m__io, this, m__root));
        try {
            _t_bars->_read();
        } catch(...) {
            m_bars->push_back(std::move(_t_bars));
            throw;
        }
        m_bars->push_back(std::move(_t_bars));
    }
    m__unnamed18 = m__io->read_bytes(4 * (999 - bar_count()));
    m__unnamed19 = m__io->read_bytes(865);
    m_events = std::unique_ptr<std::vector<std::unique_ptr<event_t>>>(new std::vector<std::unique_ptr<event_t>>());
    {
        int i = 0;
        event_t* _;
        do {
            std::unique_ptr<event_t> _t_events = std::unique_ptr<event_t>(new event_t(m__io, this, m__root));
            try {
                _t_events->_read();
            } catch(...) {
                _ = _t_events.get();
                m_events->push_back(std::move(_t_events));
                throw;
            }
            _ = _t_events.get();
            m_events->push_back(std::move(_t_events));
            i++;
        } while (!(_->tick() == 1048575));
    }
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_body_t::_fetch_instances() {
    m_start_time.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_device_names->size(); ++i) {
    }
    m_tracks.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_bars->size(); ++i) {
        m_bars->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_events->size(); ++i) {
        m_events->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::sequence_body_t::_write() {
    m__io->write_u2le(static_cast<uint16_t>(m_is_used));
    m__io->write_u1(m_index);
    m__io->write_bytes(m__unnamed2);
    m__io->write_u2le(m_bar_count);
    m__io->write_u4le(m_last_tick);
    m__io->write_bytes(m__unnamed5);
    m__io->write_u2le(m_loop_start_bar_index);
    m__io->write_u2le(m_loop_end_bar_index);
    m__io->write_bits_int_le(1, ((m_loop_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed9);
    if (m_start_time.get() == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/10: nested object is not set");
    }
    m_start_time.get()->_set_io(m__io);
    m_start_time.get()->_write();
    m__io->write_bytes(m__unnamed11);
    m__io->write_u4le(m_last_tick2);
    m__io->write_bytes(m__unnamed13);
    if (m_device_names == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/14: repeated field is not set");
    }
    for (std::vector<std::string>::const_iterator it = m_device_names->begin(); it != m_device_names->end(); ++it) {
        m__io->write_bytes((*it));
    }
    if (m_tracks.get() == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/15: nested object is not set");
    }
    m_tracks.get()->_set_io(m__io);
    m_tracks.get()->_write();
    m__io->write_bytes(m__unnamed16);
    if (m_bars == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/17: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<bar_t>>::const_iterator it = m_bars->begin(); it != m_bars->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence_body/seq/17: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed18);
    m__io->write_bytes(m__unnamed19);
    if (m_events == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/20: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<event_t>>::const_iterator it = m_events->begin(); it != m_events->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence_body/seq/20: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_body_t::_check() {
    if (m__unnamed2.size() != static_cast<std::string::size_type>(7)) {
        throw std::runtime_error("/types/sequence_body/seq/2: size mismatch");
    }
    if (m__unnamed5.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/sequence_body/seq/5: size mismatch");
    }
    if (m_start_time.get() == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/10: nested object is not set");
    }
    m_start_time.get()->_set_io(m__io);
    m_start_time.get()->_check();
    if (m__unnamed11.size() != static_cast<std::string::size_type>(6)) {
        throw std::runtime_error("/types/sequence_body/seq/11: size mismatch");
    }
    if (m__unnamed13.size() != static_cast<std::string::size_type>(52)) {
        throw std::runtime_error("/types/sequence_body/seq/13: size mismatch");
    }
    if (m_device_names == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/14: repeated field is not set");
    }
    if (m_device_names->size() != static_cast<std::size_t>(33)) {
        throw std::runtime_error("/types/sequence_body/seq/14: repeat-expr size mismatch");
    }
    for (std::vector<std::string>::const_iterator it = m_device_names->begin(); it != m_device_names->end(); ++it) {
        if ((*it).size() != static_cast<std::string::size_type>(8)) {
            throw std::runtime_error("/types/sequence_body/seq/14: size mismatch");
        }
    }
    if (m_tracks.get() == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/15: nested object is not set");
    }
    m_tracks.get()->_set_io(m__io);
    m_tracks.get()->_check();
    if (m__unnamed16.size() != static_cast<std::string::size_type>(3587)) {
        throw std::runtime_error("/types/sequence_body/seq/16: size mismatch");
    }
    if (m_bars == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/17: repeated field is not set");
    }
    if (m_bars->size() != static_cast<std::size_t>(bar_count())) {
        throw std::runtime_error("/types/sequence_body/seq/17: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<bar_t>>::const_iterator it = m_bars->begin(); it != m_bars->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence_body/seq/17: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed18.size() != static_cast<std::string::size_type>(4 * (999 - bar_count()))) {
        throw std::runtime_error("/types/sequence_body/seq/18: size mismatch");
    }
    if (m__unnamed19.size() != static_cast<std::string::size_type>(865)) {
        throw std::runtime_error("/types/sequence_body/seq/19: size mismatch");
    }
    if (m_events == nullptr) {
        throw std::runtime_error("/types/sequence_body/seq/20: repeated field is not set");
    }
    if (m_events->empty()) {
        throw std::runtime_error("/types/sequence_body/seq/20: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<event_t>>::const_iterator it = m_events->begin(); it != m_events->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_events->begin());
        const event_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence_body/seq/20: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_events->size() - 1);
        if ((_->tick() == 1048575) != _is_last) {
            throw std::runtime_error("/types/sequence_body/seq/20: repeat-until condition mismatch");
        }
    }
    m__dirty = false;
}

mpc2000xl_all_t::sequence_body_t::~sequence_body_t() {}

mpc2000xl_all_t::sequence_body_t::start_time_t::start_time_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_body_t::start_time_t::_read() {
    m_hours = m__io->read_u1();
    m_minutes = m__io->read_u1();
    m_seconds = m__io->read_u1();
    m_frames = m__io->read_u1();
    m_frame_decimals = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_body_t::start_time_t::_fetch_instances() {
}

void mpc2000xl_all_t::sequence_body_t::start_time_t::_write() {
    m__io->write_u1(m_hours);
    m__io->write_u1(m_minutes);
    m__io->write_u1(m_seconds);
    m__io->write_u1(m_frames);
    m__io->write_u1(m_frame_decimals);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_body_t::start_time_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::sequence_body_t::start_time_t::~start_time_t() {}

mpc2000xl_all_t::sequence_meta_t::sequence_meta_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_is_used = false;
}

void mpc2000xl_all_t::sequence_meta_t::_read() {
    m_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m_last_event_index = m__io->read_u2le();
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_meta_t::_fetch_instances() {
}

void mpc2000xl_all_t::sequence_meta_t::_write() {
    m__io->write_bytes(m_name);
    m__io->write_u2le(m_last_event_index);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::sequence_meta_t::_check() {
    if (m_name.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/sequence_meta/seq/0: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::sequence_meta_t::~sequence_meta_t() {}

bool mpc2000xl_all_t::sequence_meta_t::is_used() {
    if (f_is_used)
        return m_is_used;
    f_is_used = true;
    m_is_used = last_event_index() != 0;
    return m_is_used;
}

mpc2000xl_all_t::sequencer_t::sequencer_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::sequencer_t::_read() {
    m_active_sequence = m__io->read_u1();
    m__unnamed1 = m__io->read_bytes(1);
    m_active_track = m__io->read_u1();
    m__unnamed3 = m__io->read_bytes(1);
    m_master_tempo = m__io->read_u2le();
    m_tempo_source_is_sequence = m__io->read_bits_int_le(1);
    m__unnamed6 = m__io->read_bits_int_le(7);
    m_timing_correct = static_cast<mpc2000xl_all_t::timing_correct_t>(m__io->read_u1());
    m_time_display_style = static_cast<mpc2000xl_all_t::time_display_style_t>(m__io->read_bits_int_le(1));
    m__unnamed9 = m__io->read_bits_int_le(7);
    m_second_sequence_enabled = m__io->read_bits_int_le(1);
    m__unnamed11 = m__io->read_bits_int_le(7);
    m_sequence_sequence_index = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_all_t::sequencer_t::_fetch_instances() {
}

void mpc2000xl_all_t::sequencer_t::_write() {
    m__io->write_u1(m_active_sequence);
    m__io->write_bytes(m__unnamed1);
    m__io->write_u1(m_active_track);
    m__io->write_bytes(m__unnamed3);
    m__io->write_u2le(m_master_tempo);
    m__io->write_bits_int_le(1, ((m_tempo_source_is_sequence) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed6);
    m__io->write_u1(static_cast<uint8_t>(m_timing_correct));
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_time_display_style));
    m__io->write_bits_int_le(7, m__unnamed9);
    m__io->write_bits_int_le(1, ((m_second_sequence_enabled) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed11);
    m__io->write_u1(m_sequence_sequence_index);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::sequencer_t::_check() {
    if (m__unnamed1.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/sequencer/seq/1: size mismatch");
    }
    if (m__unnamed3.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/sequencer/seq/3: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::sequencer_t::~sequencer_t() {}

mpc2000xl_all_t::song_t::song_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::song_t::_read() {
    m_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m_steps = std::unique_ptr<std::vector<std::unique_ptr<song_step_t>>>(new std::vector<std::unique_ptr<song_step_t>>());
    const int l_steps = 250;
    for (int i = 0; i < l_steps; i++) {
        std::unique_ptr<song_step_t> _t_steps = std::unique_ptr<song_step_t>(new song_step_t(m__io, this, m__root));
        try {
            _t_steps->_read();
        } catch(...) {
            m_steps->push_back(std::move(_t_steps));
            throw;
        }
        m_steps->push_back(std::move(_t_steps));
    }
    m__unnamed2 = m__io->read_bytes(2);
    m_is_used = m__io->read_bits_int_le(1);
    m_loop_first_step = m__io->read_u1();
    m_loop_last_step = m__io->read_u1();
    m_is_loop_enabled = m__io->read_bits_int_le(1);
    m__unnamed7 = m__io->read_bytes(6);
    m__dirty = false;
}

void mpc2000xl_all_t::song_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_steps->size(); ++i) {
        m_steps->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::song_t::_write() {
    m__io->write_bytes(m_name);
    if (m_steps == nullptr) {
        throw std::runtime_error("/types/song/seq/1: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<song_step_t>>::const_iterator it = m_steps->begin(); it != m_steps->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/song/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed2);
    m__io->write_bits_int_le(1, ((m_is_used) ? 1 : 0));
    m__io->write_u1(m_loop_first_step);
    m__io->write_u1(m_loop_last_step);
    m__io->write_bits_int_le(1, ((m_is_loop_enabled) ? 1 : 0));
    m__io->write_bytes(m__unnamed7);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::song_t::_check() {
    if (m_name.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/song/seq/0: size mismatch");
    }
    if (m_steps == nullptr) {
        throw std::runtime_error("/types/song/seq/1: repeated field is not set");
    }
    if (m_steps->size() != static_cast<std::size_t>(250)) {
        throw std::runtime_error("/types/song/seq/1: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<song_step_t>>::const_iterator it = m_steps->begin(); it != m_steps->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/song/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed2.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/types/song/seq/2: size mismatch");
    }
    if (m__unnamed7.size() != static_cast<std::string::size_type>(6)) {
        throw std::runtime_error("/types/song/seq/7: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::song_t::~song_t() {}

mpc2000xl_all_t::song_global_t::song_global_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::song_global_t::_read() {
    m_default_song_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m_ignore_tempo_change_events_in_sequence = m__io->read_bits_int_le(1);
    m__unnamed2 = m__io->read_bits_int_le(7);
    m__dirty = false;
}

void mpc2000xl_all_t::song_global_t::_fetch_instances() {
}

void mpc2000xl_all_t::song_global_t::_write() {
    m__io->write_bytes(m_default_song_name);
    m__io->write_bits_int_le(1, ((m_ignore_tempo_change_events_in_sequence) ? 1 : 0));
    m__io->write_bits_int_le(7, m__unnamed2);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::song_global_t::_check() {
    if (m_default_song_name.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/song_global/seq/0: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::song_global_t::~song_global_t() {}

mpc2000xl_all_t::song_step_t::song_step_t(kaitai::kstream* p__io, mpc2000xl_all_t::song_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::song_step_t::_read() {
    m_sequence_index = m__io->read_u1();
    m_repeat_count = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_all_t::song_step_t::_fetch_instances() {
}

void mpc2000xl_all_t::song_step_t::_write() {
    m__io->write_u1(m_sequence_index);
    m__io->write_u1(m_repeat_count);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::song_step_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::song_step_t::~song_step_t() {}

mpc2000xl_all_t::step_edit_options_t::step_edit_options_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::step_edit_options_t::_read() {
    m_auto_step_increment = m__io->read_bits_int_le(1);
    m_duration_of_recorded_notes = static_cast<mpc2000xl_all_t::duration_of_recorded_notes_t>(m__io->read_u1());
    m_tc_value_percentage = m__io->read_u1();
    if (!(m_tc_value_percentage >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_tc_value_percentage, m__io, std::string("/types/step_edit_options/seq/2"));
    }
    if (!(m_tc_value_percentage <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_tc_value_percentage, m__io, std::string("/types/step_edit_options/seq/2"));
    }
    m__dirty = false;
}

void mpc2000xl_all_t::step_edit_options_t::_fetch_instances() {
}

void mpc2000xl_all_t::step_edit_options_t::_write() {
    m__io->write_bits_int_le(1, ((m_auto_step_increment) ? 1 : 0));
    m__io->write_u1(static_cast<uint8_t>(m_duration_of_recorded_notes));
    m__io->write_u1(m_tc_value_percentage);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::step_edit_options_t::_check() {
    if (!(m_tc_value_percentage >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_tc_value_percentage, m__io, std::string("/types/step_edit_options/seq/2"));
    }
    if (!(m_tc_value_percentage <= 100)) {
        throw kaitai::validation_greater_than_error<uint8_t>(100, m_tc_value_percentage, m__io, std::string("/types/step_edit_options/seq/2"));
    }
    m__dirty = false;
}

mpc2000xl_all_t::step_edit_options_t::~step_edit_options_t() {}

mpc2000xl_all_t::track_status_t::track_status_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::track_status_t::_read() {
    m_unused_or_used = static_cast<mpc2000xl_all_t::unused_used_t>(m__io->read_bits_int_le(1));
    m_off_or_on = static_cast<mpc2000xl_all_t::off_on_t>(m__io->read_bits_int_le(1));
    m_transmit_program_changes = static_cast<mpc2000xl_all_t::off_on_t>(m__io->read_bits_int_le(1));
    m__unnamed3 = m__io->read_bits_int_le(5);
    m__dirty = false;
}

void mpc2000xl_all_t::track_status_t::_fetch_instances() {
}

void mpc2000xl_all_t::track_status_t::_write() {
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_unused_or_used));
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_off_or_on));
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_transmit_program_changes));
    m__io->write_bits_int_le(5, m__unnamed3);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::track_status_t::_check() {
    m__dirty = false;
}

mpc2000xl_all_t::track_status_t::~track_status_t() {}

mpc2000xl_all_t::tracks_t::tracks_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent, mpc2000xl_all_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_all_t::tracks_t::_read() {
    m_names = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    const int l_names = 64;
    for (int i = 0; i < l_names; i++) {
        m_names->push_back(std::move(kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII")));
    }
    m_device = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_device = 64;
    for (int i = 0; i < l_device; i++) {
        m_device->push_back(std::move(m__io->read_u1()));
    }
    m_bus = std::unique_ptr<std::vector<bus_t>>(new std::vector<bus_t>());
    const int l_bus = 64;
    for (int i = 0; i < l_bus; i++) {
        m_bus->push_back(std::move(static_cast<mpc2000xl_all_t::bus_t>(m__io->read_u1())));
    }
    m_program_change = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_program_change = 64;
    for (int i = 0; i < l_program_change; i++) {
        m_program_change->push_back(std::move(m__io->read_u1()));
    }
    m_velocity_ratio = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_velocity_ratio = 64;
    for (int i = 0; i < l_velocity_ratio; i++) {
        m_velocity_ratio->push_back(std::move(m__io->read_u1()));
    }
    m_status = std::unique_ptr<std::vector<std::unique_ptr<track_status_t>>>(new std::vector<std::unique_ptr<track_status_t>>());
    const int l_status = 64;
    for (int i = 0; i < l_status; i++) {
        std::unique_ptr<track_status_t> _t_status = std::unique_ptr<track_status_t>(new track_status_t(m__io, this, m__root));
        try {
            _t_status->_read();
        } catch(...) {
            m_status->push_back(std::move(_t_status));
            throw;
        }
        m_status->push_back(std::move(_t_status));
    }
    m_unknown = m__io->read_bytes(64);
    m__dirty = false;
}

void mpc2000xl_all_t::tracks_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_names->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_device->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_bus->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_program_change->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_velocity_ratio->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_status->size(); ++i) {
        m_status->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_all_t::tracks_t::_write() {
    if (m_names == nullptr) {
        throw std::runtime_error("/types/tracks/seq/0: repeated field is not set");
    }
    for (std::vector<std::string>::const_iterator it = m_names->begin(); it != m_names->end(); ++it) {
        m__io->write_bytes((*it));
    }
    if (m_device == nullptr) {
        throw std::runtime_error("/types/tracks/seq/1: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_device->begin(); it != m_device->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_bus == nullptr) {
        throw std::runtime_error("/types/tracks/seq/2: repeated field is not set");
    }
    for (std::vector<bus_t>::const_iterator it = m_bus->begin(); it != m_bus->end(); ++it) {
        m__io->write_u1(static_cast<uint8_t>((*it)));
    }
    if (m_program_change == nullptr) {
        throw std::runtime_error("/types/tracks/seq/3: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_program_change->begin(); it != m_program_change->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_velocity_ratio == nullptr) {
        throw std::runtime_error("/types/tracks/seq/4: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_velocity_ratio->begin(); it != m_velocity_ratio->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_status == nullptr) {
        throw std::runtime_error("/types/tracks/seq/5: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<track_status_t>>::const_iterator it = m_status->begin(); it != m_status->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/tracks/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m_unknown);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_all_t::tracks_t::_check() {
    if (m_names == nullptr) {
        throw std::runtime_error("/types/tracks/seq/0: repeated field is not set");
    }
    if (m_names->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/tracks/seq/0: repeat-expr size mismatch");
    }
    for (std::vector<std::string>::const_iterator it = m_names->begin(); it != m_names->end(); ++it) {
        if ((*it).size() != static_cast<std::string::size_type>(16)) {
            throw std::runtime_error("/types/tracks/seq/0: size mismatch");
        }
    }
    if (m_device == nullptr) {
        throw std::runtime_error("/types/tracks/seq/1: repeated field is not set");
    }
    if (m_device->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/tracks/seq/1: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_device->begin(); it != m_device->end(); ++it) {
    }
    if (m_bus == nullptr) {
        throw std::runtime_error("/types/tracks/seq/2: repeated field is not set");
    }
    if (m_bus->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/tracks/seq/2: repeat-expr size mismatch");
    }
    for (std::vector<bus_t>::const_iterator it = m_bus->begin(); it != m_bus->end(); ++it) {
    }
    if (m_program_change == nullptr) {
        throw std::runtime_error("/types/tracks/seq/3: repeated field is not set");
    }
    if (m_program_change->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/tracks/seq/3: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_program_change->begin(); it != m_program_change->end(); ++it) {
    }
    if (m_velocity_ratio == nullptr) {
        throw std::runtime_error("/types/tracks/seq/4: repeated field is not set");
    }
    if (m_velocity_ratio->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/tracks/seq/4: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_velocity_ratio->begin(); it != m_velocity_ratio->end(); ++it) {
    }
    if (m_status == nullptr) {
        throw std::runtime_error("/types/tracks/seq/5: repeated field is not set");
    }
    if (m_status->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/tracks/seq/5: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<track_status_t>>::const_iterator it = m_status->begin(); it != m_status->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/tracks/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_unknown.size() != static_cast<std::string::size_type>(64)) {
        throw std::runtime_error("/types/tracks/seq/6: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_all_t::tracks_t::~tracks_t() {}

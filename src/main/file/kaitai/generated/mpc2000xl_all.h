#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc2000xl_all_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <set>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc2000xl_all_t : public kaitai::kstruct {

public:
    class bar_t;
    class ch_pressure_event_t;
    class control_change_event_t;
    class count_t;
    class defaults_t;
    class event_t;
    class exclusive_event_t;
    class location_t;
    class midi_input_t;
    class midi_output_t;
    class midi_switch_t;
    class midi_sync_t;
    class misc_t;
    class mixer_event_t;
    class note_event_t;
    class pitch_bend_event_t;
    class poly_pressure_event_t;
    class program_change_event_t;
    class sequence_t;
    class sequence_body_t;
    class sequence_meta_t;
    class sequencer_t;
    class song_t;
    class song_global_t;
    class song_step_t;
    class step_edit_options_t;
    class track_status_t;
    class tracks_t;

    enum bus_t {
        BUS_MIDI = 0,
        BUS_DRUM1 = 1,
        BUS_DRUM2 = 2,
        BUS_DRUM3 = 3,
        BUS_DRUM4 = 4
    };
    static bool _is_defined_bus_t(bus_t v);

private:
    static const std::set<bus_t> _values_bus_t;

public:

    enum click_output_t {
        CLICK_OUTPUT_STEREO = 0,
        CLICK_OUTPUT_INDIV1 = 1,
        CLICK_OUTPUT_INDIV2 = 2,
        CLICK_OUTPUT_INDIV3 = 3,
        CLICK_OUTPUT_INDIV4 = 4,
        CLICK_OUTPUT_INDIV5 = 5,
        CLICK_OUTPUT_INDIV6 = 6,
        CLICK_OUTPUT_INDIV7 = 7,
        CLICK_OUTPUT_INDIV8 = 8
    };
    static bool _is_defined_click_output_t(click_output_t v);

private:
    static const std::set<click_output_t> _values_click_output_t;

public:

    enum controller_t {
        CONTROLLER_BANK_SEL_MSB = 0,
        CONTROLLER_MOD_WHEEL = 1,
        CONTROLLER_BREATH_CONT = 2,
        CONTROLLER_CC3 = 3,
        CONTROLLER_FOOT_CONTROL = 4,
        CONTROLLER_PORTA_TIME = 5,
        CONTROLLER_DATA_ENTRY = 6,
        CONTROLLER_MAIN_VOLUME = 7,
        CONTROLLER_BALANCE = 8,
        CONTROLLER_CC9 = 9,
        CONTROLLER_PAN = 10,
        CONTROLLER_EXPRESSION = 11,
        CONTROLLER_EFFECT_1 = 12,
        CONTROLLER_EFFECT_2 = 13,
        CONTROLLER_CC14 = 14,
        CONTROLLER_CC15 = 15,
        CONTROLLER_GEN_PUR_1 = 16,
        CONTROLLER_GEN_PUR_2 = 17,
        CONTROLLER_GEN_PUR_3 = 18,
        CONTROLLER_GEN_PUR_4 = 19,
        CONTROLLER_CC20 = 20,
        CONTROLLER_CC21 = 21,
        CONTROLLER_CC22 = 22,
        CONTROLLER_CC23 = 23,
        CONTROLLER_CC24 = 24,
        CONTROLLER_CC25 = 25,
        CONTROLLER_CC26 = 26,
        CONTROLLER_CC27 = 27,
        CONTROLLER_CC28 = 28,
        CONTROLLER_CC29 = 29,
        CONTROLLER_CC30 = 30,
        CONTROLLER_CC31 = 31,
        CONTROLLER_BANK_SEL_LSB = 32,
        CONTROLLER_MOD_WHEL_LSB = 33,
        CONTROLLER_BREATH_LSB = 34,
        CONTROLLER_CC35 = 35,
        CONTROLLER_FOOT_CNT_LSB = 36,
        CONTROLLER_PORT_TIME_LS = 37,
        CONTROLLER_DATA_ENT_LSB = 38,
        CONTROLLER_MAIN_VOL_LSB = 39,
        CONTROLLER_BALANCE_LSB = 40,
        CONTROLLER_CC41 = 41,
        CONTROLLER_PAN_LSB = 42,
        CONTROLLER_EXPRESS_LSB = 43,
        CONTROLLER_EFFECT_1_LSB = 44,
        CONTROLLER_EFFECT_2_MSB = 45,
        CONTROLLER_CC46 = 46,
        CONTROLLER_CC47 = 47,
        CONTROLLER_GEN_PUR_1_LS = 48,
        CONTROLLER_GEN_PUR_2_LS = 49,
        CONTROLLER_GEN_PUR_3_LS = 50,
        CONTROLLER_GEN_PUR_4_LS = 51,
        CONTROLLER_CC52 = 52,
        CONTROLLER_CC53 = 53,
        CONTROLLER_CC54 = 54,
        CONTROLLER_CC55 = 55,
        CONTROLLER_CC56 = 56,
        CONTROLLER_CC57 = 57,
        CONTROLLER_CC58 = 58,
        CONTROLLER_CC59 = 59,
        CONTROLLER_CC60 = 60,
        CONTROLLER_CC61 = 61,
        CONTROLLER_CC62 = 62,
        CONTROLLER_CC63 = 63,
        CONTROLLER_SUSTAIN_PDL = 64,
        CONTROLLER_PORTA_PEDAL = 65,
        CONTROLLER_SOSTENUTO = 66,
        CONTROLLER_SOFT_PEDAL = 67,
        CONTROLLER_LEGATO_FT_SW = 68,
        CONTROLLER_HOLD_2 = 69,
        CONTROLLER_SOUND_VARI = 70,
        CONTROLLER_TIMBER_HARMO = 71,
        CONTROLLER_RELEASE_TIME = 72,
        CONTROLLER_ATTACK_TIME = 73,
        CONTROLLER_BRIGHTNESS = 74,
        CONTROLLER_SOUND_CONT_6 = 75,
        CONTROLLER_SOUND_CONT_7 = 76,
        CONTROLLER_SOUND_CONT_8 = 77,
        CONTROLLER_SOUND_CONT_9 = 78,
        CONTROLLER_SOUND_CONT10 = 79,
        CONTROLLER_GEN_PUR_5 = 80,
        CONTROLLER_GEN_PUR_6 = 81,
        CONTROLLER_GEN_PUR_7 = 82,
        CONTROLLER_GEN_PUR_8 = 83,
        CONTROLLER_PORTA_CNTRL = 84,
        CONTROLLER_CC85 = 85,
        CONTROLLER_CC86 = 86,
        CONTROLLER_CC87 = 87,
        CONTROLLER_CC88 = 88,
        CONTROLLER_CC89 = 89,
        CONTROLLER_CC90 = 90,
        CONTROLLER_EXT_EFF_DPTH = 91,
        CONTROLLER_TREMOLO_DPTH = 92,
        CONTROLLER_CHORUS_DEPTH = 93,
        CONTROLLER_DETUNE_DEPTH = 94,
        CONTROLLER_PHASER_DEPTH = 95,
        CONTROLLER_DATA_INCRE = 96,
        CONTROLLER_DATA_DECRE = 97,
        CONTROLLER_NRPN_LSB = 98,
        CONTROLLER_NRPN_MSB = 99,
        CONTROLLER_RPN_LSB = 100,
        CONTROLLER_RPN_MSB = 101,
        CONTROLLER_CC102 = 102,
        CONTROLLER_CC103 = 103,
        CONTROLLER_CC104 = 104,
        CONTROLLER_CC105 = 105,
        CONTROLLER_CC106 = 106,
        CONTROLLER_CC107 = 107,
        CONTROLLER_CC108 = 108,
        CONTROLLER_CC109 = 109,
        CONTROLLER_CC110 = 110,
        CONTROLLER_CC111 = 111,
        CONTROLLER_CC112 = 112,
        CONTROLLER_CC113 = 113,
        CONTROLLER_CC114 = 114,
        CONTROLLER_CC115 = 115,
        CONTROLLER_CC116 = 116,
        CONTROLLER_CC117 = 117,
        CONTROLLER_CC118 = 118,
        CONTROLLER_CC119 = 119,
        CONTROLLER_ALL_SND_OFF = 120,
        CONTROLLER_RESET_CONTRL = 121,
        CONTROLLER_LOCAL_ON_OFF = 122,
        CONTROLLER_ALL_NOTE_OFF = 123,
        CONTROLLER_OMNI_OFF = 124,
        CONTROLLER_OMNI_ON = 125,
        CONTROLLER_MONO_MODE_ON = 126,
        CONTROLLER_POLY_MODE_ON = 127
    };
    static bool _is_defined_controller_t(controller_t v);

private:
    static const std::set<controller_t> _values_controller_t;

public:

    enum count_in_mode_t {
        COUNT_IN_MODE_FALSE = 0,
        COUNT_IN_MODE_REC_ONLY = 1,
        COUNT_IN_MODE_REC_AND_PLAY = 2
    };
    static bool _is_defined_count_in_mode_t(count_in_mode_t v);

private:
    static const std::set<count_in_mode_t> _values_count_in_mode_t;

public:

    enum duration_of_recorded_notes_t {
        DURATION_OF_RECORDED_NOTES_AS_PLAYED = 0,
        DURATION_OF_RECORDED_NOTES_TC_VALUE = 1
    };
    static bool _is_defined_duration_of_recorded_notes_t(duration_of_recorded_notes_t v);

private:
    static const std::set<duration_of_recorded_notes_t> _values_duration_of_recorded_notes_t;

public:

    enum frame_rate_t {
        FRAME_RATE_FPS_24 = 0,
        FRAME_RATE_FPS_25 = 1,
        FRAME_RATE_FPS_30D = 2,
        FRAME_RATE_FPS_30 = 3
    };
    static bool _is_defined_frame_rate_t(frame_rate_t v);

private:
    static const std::set<frame_rate_t> _values_frame_rate_t;

public:

    enum midi_input_filter_type_t {
        MIDI_INPUT_FILTER_TYPE_NOTES = 0,
        MIDI_INPUT_FILTER_TYPE_PITCH_BEND = 1,
        MIDI_INPUT_FILTER_TYPE_PROG_CHANGE = 2,
        MIDI_INPUT_FILTER_TYPE_CH_PRESSURE = 3,
        MIDI_INPUT_FILTER_TYPE_POLY_PRESS = 4,
        MIDI_INPUT_FILTER_TYPE_EXCLUSIVE = 5,
        MIDI_INPUT_FILTER_TYPE_CC0_BANK_SEL_MSB = 6,
        MIDI_INPUT_FILTER_TYPE_CC1_MOD_WHEEL = 7,
        MIDI_INPUT_FILTER_TYPE_CC2_BREATH_CONT = 8,
        MIDI_INPUT_FILTER_TYPE_CC3 = 9,
        MIDI_INPUT_FILTER_TYPE_CC4_FOOT_CONTROL = 10,
        MIDI_INPUT_FILTER_TYPE_CC5_PORTA_TIME = 11,
        MIDI_INPUT_FILTER_TYPE_CC6_DATA_ENTRY = 12,
        MIDI_INPUT_FILTER_TYPE_CC7_MAIN_VOLUME = 13,
        MIDI_INPUT_FILTER_TYPE_CC8_BALANCE = 14,
        MIDI_INPUT_FILTER_TYPE_CC9 = 15,
        MIDI_INPUT_FILTER_TYPE_CC10_PAN = 16,
        MIDI_INPUT_FILTER_TYPE_CC11_EXPRESSION = 17,
        MIDI_INPUT_FILTER_TYPE_CC12_EFFECT_1 = 18,
        MIDI_INPUT_FILTER_TYPE_CC13_EFFECT_2 = 19,
        MIDI_INPUT_FILTER_TYPE_CC14 = 20,
        MIDI_INPUT_FILTER_TYPE_CC15 = 21,
        MIDI_INPUT_FILTER_TYPE_CC16_GEN_PUR_1 = 22,
        MIDI_INPUT_FILTER_TYPE_CC17_GEN_PUR_2 = 23,
        MIDI_INPUT_FILTER_TYPE_CC18_GEN_PUR_3 = 24,
        MIDI_INPUT_FILTER_TYPE_CC19_GEN_PUR_4 = 25,
        MIDI_INPUT_FILTER_TYPE_CC20 = 26,
        MIDI_INPUT_FILTER_TYPE_CC21 = 27,
        MIDI_INPUT_FILTER_TYPE_CC22 = 28,
        MIDI_INPUT_FILTER_TYPE_CC23 = 29,
        MIDI_INPUT_FILTER_TYPE_CC24 = 30,
        MIDI_INPUT_FILTER_TYPE_CC25 = 31,
        MIDI_INPUT_FILTER_TYPE_CC26 = 32,
        MIDI_INPUT_FILTER_TYPE_CC27 = 33,
        MIDI_INPUT_FILTER_TYPE_CC28 = 34,
        MIDI_INPUT_FILTER_TYPE_CC29 = 35,
        MIDI_INPUT_FILTER_TYPE_CC30 = 36,
        MIDI_INPUT_FILTER_TYPE_CC31 = 37,
        MIDI_INPUT_FILTER_TYPE_CC32_BANK_SEL_LSB = 38,
        MIDI_INPUT_FILTER_TYPE_CC33_MOD_WHEL_LSB = 39,
        MIDI_INPUT_FILTER_TYPE_CC34_BREATH_LSB = 40,
        MIDI_INPUT_FILTER_TYPE_CC35 = 41,
        MIDI_INPUT_FILTER_TYPE_CC36_FOOT_CNT_LSB = 42,
        MIDI_INPUT_FILTER_TYPE_CC37_PORT_TIME_LS = 43,
        MIDI_INPUT_FILTER_TYPE_CC38_DATA_ENT_LSB = 44,
        MIDI_INPUT_FILTER_TYPE_CC39_MAIN_VOL_LSB = 45,
        MIDI_INPUT_FILTER_TYPE_CC40_BALANCE_LSB = 46,
        MIDI_INPUT_FILTER_TYPE_CC41 = 47,
        MIDI_INPUT_FILTER_TYPE_CC42_PAN_LSB = 48,
        MIDI_INPUT_FILTER_TYPE_CC43_EXPRESS_LSB = 49,
        MIDI_INPUT_FILTER_TYPE_CC44_EFFECT_1_LSB = 50,
        MIDI_INPUT_FILTER_TYPE_CC45_EFFECT_2_MSB = 51,
        MIDI_INPUT_FILTER_TYPE_CC46 = 52,
        MIDI_INPUT_FILTER_TYPE_CC47 = 53,
        MIDI_INPUT_FILTER_TYPE_CC48_GEN_PUR_1_LS = 54,
        MIDI_INPUT_FILTER_TYPE_CC49_GEN_PUR_2_LS = 55,
        MIDI_INPUT_FILTER_TYPE_CC50_GEN_PUR_3_LS = 56,
        MIDI_INPUT_FILTER_TYPE_CC51_GEN_PUR_4_LS = 57,
        MIDI_INPUT_FILTER_TYPE_CC52 = 58,
        MIDI_INPUT_FILTER_TYPE_CC53 = 59,
        MIDI_INPUT_FILTER_TYPE_CC54 = 60,
        MIDI_INPUT_FILTER_TYPE_CC55 = 61,
        MIDI_INPUT_FILTER_TYPE_CC56 = 62,
        MIDI_INPUT_FILTER_TYPE_CC57 = 63,
        MIDI_INPUT_FILTER_TYPE_CC58 = 64,
        MIDI_INPUT_FILTER_TYPE_CC59 = 65,
        MIDI_INPUT_FILTER_TYPE_CC60 = 66,
        MIDI_INPUT_FILTER_TYPE_CC61 = 67,
        MIDI_INPUT_FILTER_TYPE_CC62 = 68,
        MIDI_INPUT_FILTER_TYPE_CC63 = 69,
        MIDI_INPUT_FILTER_TYPE_CC64_SUSTAIN_PDL = 70,
        MIDI_INPUT_FILTER_TYPE_CC65_PORTA_PEDAL = 71,
        MIDI_INPUT_FILTER_TYPE_CC66_SOSTENUTO = 72,
        MIDI_INPUT_FILTER_TYPE_CC67_SOFT_PEDAL = 73,
        MIDI_INPUT_FILTER_TYPE_CC68_LEGATO_FT_SW = 74,
        MIDI_INPUT_FILTER_TYPE_CC69_HOLD_2 = 75,
        MIDI_INPUT_FILTER_TYPE_CC70_SOUND_VARI = 76,
        MIDI_INPUT_FILTER_TYPE_CC71_TIMBER_HARMO = 77,
        MIDI_INPUT_FILTER_TYPE_CC72_RELEASE_TIME = 78,
        MIDI_INPUT_FILTER_TYPE_CC73_ATTACK_TIME = 79,
        MIDI_INPUT_FILTER_TYPE_CC74_BRIGHTNESS = 80,
        MIDI_INPUT_FILTER_TYPE_CC75_SOUND_CONT_6 = 81,
        MIDI_INPUT_FILTER_TYPE_CC76_SOUND_CONT_7 = 82,
        MIDI_INPUT_FILTER_TYPE_CC77_SOUND_CONT_8 = 83,
        MIDI_INPUT_FILTER_TYPE_CC78_SOUND_CONT_9 = 84,
        MIDI_INPUT_FILTER_TYPE_CC79_SOUND_CONT10 = 85,
        MIDI_INPUT_FILTER_TYPE_CC80_GEN_PUR_5 = 86,
        MIDI_INPUT_FILTER_TYPE_CC81_GEN_PUR_6 = 87,
        MIDI_INPUT_FILTER_TYPE_CC82_GEN_PUR_7 = 88,
        MIDI_INPUT_FILTER_TYPE_CC83_GEN_PUR_8 = 89,
        MIDI_INPUT_FILTER_TYPE_CC84_PORTA_CNTRL = 90,
        MIDI_INPUT_FILTER_TYPE_CC85 = 91,
        MIDI_INPUT_FILTER_TYPE_CC86 = 92,
        MIDI_INPUT_FILTER_TYPE_CC87 = 93,
        MIDI_INPUT_FILTER_TYPE_CC88 = 94,
        MIDI_INPUT_FILTER_TYPE_CC89 = 95,
        MIDI_INPUT_FILTER_TYPE_CC90 = 96,
        MIDI_INPUT_FILTER_TYPE_CC91_EXT_EFF_DPTH = 97,
        MIDI_INPUT_FILTER_TYPE_CC92_TREMOLO_DPTH = 98,
        MIDI_INPUT_FILTER_TYPE_CC93_CHORUS_DEPTH = 99,
        MIDI_INPUT_FILTER_TYPE_CC94_DETUNE_DEPTH = 100,
        MIDI_INPUT_FILTER_TYPE_CC95_PHASER_DEPTH = 101,
        MIDI_INPUT_FILTER_TYPE_CC96_DATA_INCRE = 102,
        MIDI_INPUT_FILTER_TYPE_CC97_DATA_DECRE = 103,
        MIDI_INPUT_FILTER_TYPE_CC98_NRPN_LSB = 104,
        MIDI_INPUT_FILTER_TYPE_CC99_NRPN_MSB = 105,
        MIDI_INPUT_FILTER_TYPE_CC100_RPN_LSB = 106,
        MIDI_INPUT_FILTER_TYPE_CC101_RPN_MSB = 107,
        MIDI_INPUT_FILTER_TYPE_CC102 = 108,
        MIDI_INPUT_FILTER_TYPE_CC103 = 109,
        MIDI_INPUT_FILTER_TYPE_CC104 = 110,
        MIDI_INPUT_FILTER_TYPE_CC105 = 111,
        MIDI_INPUT_FILTER_TYPE_CC106 = 112,
        MIDI_INPUT_FILTER_TYPE_CC107 = 113,
        MIDI_INPUT_FILTER_TYPE_CC108 = 114,
        MIDI_INPUT_FILTER_TYPE_CC109 = 115,
        MIDI_INPUT_FILTER_TYPE_CC110 = 116,
        MIDI_INPUT_FILTER_TYPE_CC111 = 117,
        MIDI_INPUT_FILTER_TYPE_CC112 = 118,
        MIDI_INPUT_FILTER_TYPE_CC113 = 119,
        MIDI_INPUT_FILTER_TYPE_CC114 = 120,
        MIDI_INPUT_FILTER_TYPE_CC115 = 121,
        MIDI_INPUT_FILTER_TYPE_CC116 = 122,
        MIDI_INPUT_FILTER_TYPE_CC117 = 123,
        MIDI_INPUT_FILTER_TYPE_CC118 = 124,
        MIDI_INPUT_FILTER_TYPE_CC119 = 125,
        MIDI_INPUT_FILTER_TYPE_CC120_ALL_SND_OFF = 126,
        MIDI_INPUT_FILTER_TYPE_CC121_RESET_CONTRL = 127,
        MIDI_INPUT_FILTER_TYPE_CC122_LOCAL_ON_OFF = 128,
        MIDI_INPUT_FILTER_TYPE_CC123_ALL_NOTE_OFF = 129,
        MIDI_INPUT_FILTER_TYPE_CC124_OMNI_OFF = 130,
        MIDI_INPUT_FILTER_TYPE_CC125_OMNI_ON = 131,
        MIDI_INPUT_FILTER_TYPE_CC126_MONO_MODE_ON = 132,
        MIDI_INPUT_FILTER_TYPE_CC127_POLY_MODE_ON = 133
    };
    static bool _is_defined_midi_input_filter_type_t(midi_input_filter_type_t v);

private:
    static const std::set<midi_input_filter_type_t> _values_midi_input_filter_type_t;

public:

    enum midi_input_receive_channel_t {
        MIDI_INPUT_RECEIVE_CHANNEL_ALL = 0,
        MIDI_INPUT_RECEIVE_CHANNEL_CH1 = 1,
        MIDI_INPUT_RECEIVE_CHANNEL_CH2 = 2,
        MIDI_INPUT_RECEIVE_CHANNEL_CH3 = 3,
        MIDI_INPUT_RECEIVE_CHANNEL_CH4 = 4,
        MIDI_INPUT_RECEIVE_CHANNEL_CH5 = 5,
        MIDI_INPUT_RECEIVE_CHANNEL_CH6 = 6,
        MIDI_INPUT_RECEIVE_CHANNEL_CH7 = 7,
        MIDI_INPUT_RECEIVE_CHANNEL_CH8 = 8,
        MIDI_INPUT_RECEIVE_CHANNEL_CH9 = 9,
        MIDI_INPUT_RECEIVE_CHANNEL_CH10 = 10,
        MIDI_INPUT_RECEIVE_CHANNEL_CH11 = 11,
        MIDI_INPUT_RECEIVE_CHANNEL_CH12 = 12,
        MIDI_INPUT_RECEIVE_CHANNEL_CH13 = 13,
        MIDI_INPUT_RECEIVE_CHANNEL_CH14 = 14,
        MIDI_INPUT_RECEIVE_CHANNEL_CH15 = 15,
        MIDI_INPUT_RECEIVE_CHANNEL_CH16 = 16
    };
    static bool _is_defined_midi_input_receive_channel_t(midi_input_receive_channel_t v);

private:
    static const std::set<midi_input_receive_channel_t> _values_midi_input_receive_channel_t;

public:

    enum midi_switch_function_t {
        MIDI_SWITCH_FUNCTION_PLAY_STRT = 0,
        MIDI_SWITCH_FUNCTION_PLAY = 1,
        MIDI_SWITCH_FUNCTION_STOP = 2,
        MIDI_SWITCH_FUNCTION_REC_AND_PLAY = 3,
        MIDI_SWITCH_FUNCTION_ODUB_AND_PLAY = 4,
        MIDI_SWITCH_FUNCTION_REC_PUNCH = 5,
        MIDI_SWITCH_FUNCTION_ODUB_PNCH = 6,
        MIDI_SWITCH_FUNCTION_TAP = 7,
        MIDI_SWITCH_FUNCTION_PAD_BNK_A = 8,
        MIDI_SWITCH_FUNCTION_PAD_BNK_B = 9,
        MIDI_SWITCH_FUNCTION_PAD_BNK_C = 10,
        MIDI_SWITCH_FUNCTION_PAD_BNK_D = 11,
        MIDI_SWITCH_FUNCTION_PAD_1 = 12,
        MIDI_SWITCH_FUNCTION_PAD_2 = 13,
        MIDI_SWITCH_FUNCTION_PAD_3 = 14,
        MIDI_SWITCH_FUNCTION_PAD_4 = 15,
        MIDI_SWITCH_FUNCTION_PAD_5 = 16,
        MIDI_SWITCH_FUNCTION_PAD_6 = 17,
        MIDI_SWITCH_FUNCTION_PAD_7 = 18,
        MIDI_SWITCH_FUNCTION_PAD_8 = 19,
        MIDI_SWITCH_FUNCTION_PAD_9 = 20,
        MIDI_SWITCH_FUNCTION_PAD_10 = 21,
        MIDI_SWITCH_FUNCTION_PAD_11 = 22,
        MIDI_SWITCH_FUNCTION_PAD_12 = 23,
        MIDI_SWITCH_FUNCTION_PAD_13 = 24,
        MIDI_SWITCH_FUNCTION_PAD_14 = 25,
        MIDI_SWITCH_FUNCTION_PAD_15 = 26,
        MIDI_SWITCH_FUNCTION_PAD_16 = 27,
        MIDI_SWITCH_FUNCTION_F1 = 28,
        MIDI_SWITCH_FUNCTION_F2 = 29,
        MIDI_SWITCH_FUNCTION_F3 = 30,
        MIDI_SWITCH_FUNCTION_F4 = 31,
        MIDI_SWITCH_FUNCTION_F5 = 32,
        MIDI_SWITCH_FUNCTION_F6 = 33
    };
    static bool _is_defined_midi_switch_function_t(midi_switch_function_t v);

private:
    static const std::set<midi_switch_function_t> _values_midi_switch_function_t;

public:

    enum midi_sync_mode_t {
        MIDI_SYNC_MODE_FALSE = 0,
        MIDI_SYNC_MODE_MIDI_CLOCK = 1,
        MIDI_SYNC_MODE_TIME_CODE = 2
    };
    static bool _is_defined_midi_sync_mode_t(midi_sync_mode_t v);

private:
    static const std::set<midi_sync_mode_t> _values_midi_sync_mode_t;

public:

    enum midi_sync_output_t {
        MIDI_SYNC_OUTPUT_A = 0,
        MIDI_SYNC_OUTPUT_B = 1,
        MIDI_SYNC_OUTPUT_A_AND_B = 2
    };
    static bool _is_defined_midi_sync_output_t(midi_sync_output_t v);

private:
    static const std::set<midi_sync_output_t> _values_midi_sync_output_t;

public:

    enum mixer_event_param_t {
        MIXER_EVENT_PARAM_STEREO_LEVEL = 1,
        MIXER_EVENT_PARAM_STEREO_PAN = 2,
        MIXER_EVENT_PARAM_FXSEND_LEVEL = 3,
        MIXER_EVENT_PARAM_INDIV_LEVEL = 5
    };
    static bool _is_defined_mixer_event_param_t(mixer_event_param_t v);

private:
    static const std::set<mixer_event_param_t> _values_mixer_event_param_t;

public:

    enum off_on_t {
        OFF_ON_FALSE = 0,
        OFF_ON_TRUE = 1
    };
    static bool _is_defined_off_on_t(off_on_t v);

private:
    static const std::set<off_on_t> _values_off_on_t;

public:

    enum rate_t {
        RATE_ONE_4 = 0,
        RATE_ONE_4_3 = 1,
        RATE_ONE_8 = 2,
        RATE_ONE_8_3 = 3,
        RATE_ONE_16 = 4,
        RATE_ONE_16_3 = 5,
        RATE_ONE_32 = 6,
        RATE_ONE_32_3 = 7
    };
    static bool _is_defined_rate_t(rate_t v);

private:
    static const std::set<rate_t> _values_rate_t;

public:

    enum sequence_is_used_t {
        SEQUENCE_IS_USED_FALSE = 0,
        SEQUENCE_IS_USED_TRUE = 641
    };
    static bool _is_defined_sequence_is_used_t(sequence_is_used_t v);

private:
    static const std::set<sequence_is_used_t> _values_sequence_is_used_t;

public:

    enum soft_thru_mode_t {
        SOFT_THRU_MODE_FALSE = 0,
        SOFT_THRU_MODE_AS_TRACK = 1,
        SOFT_THRU_MODE_OMNI_A = 2,
        SOFT_THRU_MODE_OMNI_B = 3,
        SOFT_THRU_MODE_OMNI_AB = 4
    };
    static bool _is_defined_soft_thru_mode_t(soft_thru_mode_t v);

private:
    static const std::set<soft_thru_mode_t> _values_soft_thru_mode_t;

public:

    enum sound_source_t {
        SOUND_SOURCE_CLICK = 0,
        SOUND_SOURCE_DRUM1 = 1,
        SOUND_SOURCE_DRUM2 = 2,
        SOUND_SOURCE_DRUM3 = 3,
        SOUND_SOURCE_DRUM4 = 4
    };
    static bool _is_defined_sound_source_t(sound_source_t v);

private:
    static const std::set<sound_source_t> _values_sound_source_t;

public:

    enum tap_averaging_t {
        TAP_AVERAGING_TAP_AVG_2 = 0,
        TAP_AVERAGING_TAP_AVG_3 = 1,
        TAP_AVERAGING_TAP_AVG_4 = 2
    };
    static bool _is_defined_tap_averaging_t(tap_averaging_t v);

private:
    static const std::set<tap_averaging_t> _values_tap_averaging_t;

public:

    enum time_display_style_t {
        TIME_DISPLAY_STYLE_BAR_BEAT_CLOCK = 0,
        TIME_DISPLAY_STYLE_HOUR_MINUTE_SEC = 1
    };
    static bool _is_defined_time_display_style_t(time_display_style_t v);

private:
    static const std::set<time_display_style_t> _values_time_display_style_t;

public:

    enum timing_correct_t {
        TIMING_CORRECT_FALSE = 0,
        TIMING_CORRECT_ONE_8 = 1,
        TIMING_CORRECT_ONE_8_3 = 2,
        TIMING_CORRECT_ONE_16 = 3,
        TIMING_CORRECT_ONE_16_3 = 4,
        TIMING_CORRECT_ONE_32 = 5,
        TIMING_CORRECT_ONE_32_3 = 6
    };
    static bool _is_defined_timing_correct_t(timing_correct_t v);

private:
    static const std::set<timing_correct_t> _values_timing_correct_t;

public:

    enum unused_used_t {
        UNUSED_USED_UNUSED = 0,
        UNUSED_USED_USED = 1
    };
    static bool _is_defined_unused_used_t(unused_used_t v);

private:
    static const std::set<unused_used_t> _values_unused_used_t;

public:

private:
    bool m__dirty;

public:

    mpc2000xl_all_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc2000xl_all_t();

    class bar_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        bar_t(int32_t p_idx, kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~bar_t();
        int32_t denominator();
        void _invalidate_denominator() { f_denominator = false; }
        int32_t first_tick();
        void _invalidate_first_tick() { f_first_tick = false; }
        int32_t numerator();
        void _invalidate_numerator() { f_numerator = false; }
        uint8_t ticks_per_beat() const { return m_ticks_per_beat; }
        void set_ticks_per_beat(uint8_t _v) { m__dirty = true; f_denominator = false; f_first_tick = false; f_numerator = false; m_ticks_per_beat = std::move(_v); }
        uint64_t last_tick() const { return m_last_tick; }
        void set_last_tick(uint64_t _v) { m__dirty = true; f_denominator = false; f_first_tick = false; f_numerator = false; m_last_tick = std::move(_v); }
        int32_t idx() const { return m_idx; }
        void set_idx(int32_t _v) { m__dirty = true; f_denominator = false; f_first_tick = false; f_numerator = false; m_idx = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; f_denominator = false; f_first_tick = false; f_numerator = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::sequence_body_t* _v) { m__dirty = true; f_denominator = false; f_first_tick = false; f_numerator = false; m__parent = std::move(_v); }

    private:
        bool f_denominator;
        int32_t m_denominator;
        bool f_first_tick;
        int32_t m_first_tick;
        bool f_numerator;
        int32_t m_numerator;
        uint8_t m_ticks_per_beat;
        uint64_t m_last_tick;
        int32_t m_idx;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::sequence_body_t* m__parent;
    };

    class ch_pressure_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        ch_pressure_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~ch_pressure_event_t();
        uint8_t pressure() const { return m_pressure; }
        void set_pressure(uint8_t _v) { m__dirty = true; m_pressure = std::move(_v); }
        std::string _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(std::string _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_pressure;
        std::string m__unnamed1;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::event_t* m__parent;
    };

    class control_change_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        control_change_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~control_change_event_t();
        controller_t controller() const { return m_controller; }
        void set_controller(controller_t _v) { m__dirty = true; m_controller = std::move(_v); }
        uint8_t value() const { return m_value; }
        void set_value(uint8_t _v) { m__dirty = true; m_value = std::move(_v); }
        std::string _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(std::string _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        controller_t m_controller;
        uint8_t m_value;
        std::string m__unnamed2;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::event_t* m__parent;
    };

    class count_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        count_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~count_t();
        bool enabled() const { return m_enabled; }
        void set_enabled(bool _v) { m__dirty = true; m_enabled = std::move(_v); }
        uint64_t _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(uint64_t _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        count_in_mode_t count_in_mode() const { return m_count_in_mode; }
        void set_count_in_mode(count_in_mode_t _v) { m__dirty = true; m_count_in_mode = std::move(_v); }
        uint8_t click_volume() const { return m_click_volume; }
        void set_click_volume(uint8_t _v) { m__dirty = true; m_click_volume = std::move(_v); }
        rate_t rate() const { return m_rate; }
        void set_rate(rate_t _v) { m__dirty = true; m_rate = std::move(_v); }
        bool enabled_in_play() const { return m_enabled_in_play; }
        void set_enabled_in_play(bool _v) { m__dirty = true; m_enabled_in_play = std::move(_v); }
        uint64_t _unnamed6() const { return m__unnamed6; }
        void set__unnamed6(uint64_t _v) { m__dirty = true; m__unnamed6 = std::move(_v); }
        bool enabled_in_rec() const { return m_enabled_in_rec; }
        void set_enabled_in_rec(bool _v) { m__dirty = true; m_enabled_in_rec = std::move(_v); }
        uint64_t _unnamed8() const { return m__unnamed8; }
        void set__unnamed8(uint64_t _v) { m__dirty = true; m__unnamed8 = std::move(_v); }
        click_output_t click_output() const { return m_click_output; }
        void set_click_output(click_output_t _v) { m__dirty = true; m_click_output = std::move(_v); }
        bool wait_for_key() const { return m_wait_for_key; }
        void set_wait_for_key(bool _v) { m__dirty = true; m_wait_for_key = std::move(_v); }
        uint64_t _unnamed11() const { return m__unnamed11; }
        void set__unnamed11(uint64_t _v) { m__dirty = true; m__unnamed11 = std::move(_v); }
        sound_source_t sound_source() const { return m_sound_source; }
        void set_sound_source(sound_source_t _v) { m__dirty = true; m_sound_source = std::move(_v); }
        uint8_t accent_pad_index() const { return m_accent_pad_index; }
        void set_accent_pad_index(uint8_t _v) { m__dirty = true; m_accent_pad_index = std::move(_v); }
        uint8_t normal_pad_index() const { return m_normal_pad_index; }
        void set_normal_pad_index(uint8_t _v) { m__dirty = true; m_normal_pad_index = std::move(_v); }
        uint8_t accent_velo() const { return m_accent_velo; }
        void set_accent_velo(uint8_t _v) { m__dirty = true; m_accent_velo = std::move(_v); }
        uint8_t normal_velo() const { return m_normal_velo; }
        void set_normal_velo(uint8_t _v) { m__dirty = true; m_normal_velo = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        bool m_enabled;
        uint64_t m__unnamed1;
        count_in_mode_t m_count_in_mode;
        uint8_t m_click_volume;
        rate_t m_rate;
        bool m_enabled_in_play;
        uint64_t m__unnamed6;
        bool m_enabled_in_rec;
        uint64_t m__unnamed8;
        click_output_t m_click_output;
        bool m_wait_for_key;
        uint64_t m__unnamed11;
        sound_source_t m_sound_source;
        uint8_t m_accent_pad_index;
        uint8_t m_normal_pad_index;
        uint8_t m_accent_velo;
        uint8_t m_normal_velo;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class defaults_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        defaults_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~defaults_t();
        std::string sequence_name() const { return m_sequence_name; }
        void set_sequence_name(std::string _v) { m__dirty = true; m_sequence_name = std::move(_v); }
        std::string _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(std::string _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        uint16_t tempo() const { return m_tempo; }
        void set_tempo(uint16_t _v) { m__dirty = true; m_tempo = std::move(_v); }
        uint8_t numerator() const { return m_numerator; }
        void set_numerator(uint8_t _v) { m__dirty = true; m_numerator = std::move(_v); }
        uint8_t denominator() const { return m_denominator; }
        void set_denominator(uint8_t _v) { m__dirty = true; m_denominator = std::move(_v); }
        uint16_t bar_count() const { return m_bar_count; }
        void set_bar_count(uint16_t _v) { m__dirty = true; m_bar_count = std::move(_v); }
        uint16_t tick_count() const { return m_tick_count; }
        void set_tick_count(uint16_t _v) { m__dirty = true; m_tick_count = std::move(_v); }
        std::vector<uint32_t>* unknown1() const { return m_unknown1.get(); }
        void set_unknown1(std::unique_ptr<std::vector<uint32_t>> _v) { m__dirty = true; m_unknown1 = std::move(_v); }
        std::string unknown2() const { return m_unknown2; }
        void set_unknown2(std::string _v) { m__dirty = true; m_unknown2 = std::move(_v); }
        std::vector<std::string>* device_names() const { return m_device_names.get(); }
        void set_device_names(std::unique_ptr<std::vector<std::string>> _v) { m__dirty = true; m_device_names = std::move(_v); }
        std::vector<std::string>* track_names() const { return m_track_names.get(); }
        void set_track_names(std::unique_ptr<std::vector<std::string>> _v) { m__dirty = true; m_track_names = std::move(_v); }
        std::vector<uint8_t>* devices() const { return m_devices.get(); }
        void set_devices(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_devices = std::move(_v); }
        std::vector<bus_t>* buses() const { return m_buses.get(); }
        void set_buses(std::unique_ptr<std::vector<bus_t>> _v) { m__dirty = true; m_buses = std::move(_v); }
        std::vector<uint8_t>* programs() const { return m_programs.get(); }
        void set_programs(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_programs = std::move(_v); }
        std::vector<uint8_t>* track_velocities() const { return m_track_velocities.get(); }
        void set_track_velocities(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_track_velocities = std::move(_v); }
        std::vector<std::unique_ptr<track_status_t>>* track_statuses() const { return m_track_statuses.get(); }
        void set_track_statuses(std::unique_ptr<std::vector<std::unique_ptr<track_status_t>>> _v) { m__dirty = true; m_track_statuses = std::move(_v); }
        std::string _unnamed16() const { return m__unnamed16; }
        void set__unnamed16(std::string _v) { m__dirty = true; m__unnamed16 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m_sequence_name;
        std::string m__unnamed1;
        uint16_t m_tempo;
        uint8_t m_numerator;
        uint8_t m_denominator;
        uint16_t m_bar_count;
        uint16_t m_tick_count;
        std::unique_ptr<std::vector<uint32_t>> m_unknown1;
        std::string m_unknown2;
        std::unique_ptr<std::vector<std::string>> m_device_names;
        std::unique_ptr<std::vector<std::string>> m_track_names;
        std::unique_ptr<std::vector<uint8_t>> m_devices;
        std::unique_ptr<std::vector<bus_t>> m_buses;
        std::unique_ptr<std::vector<uint8_t>> m_programs;
        std::unique_ptr<std::vector<uint8_t>> m_track_velocities;
        std::unique_ptr<std::vector<std::unique_ptr<track_status_t>>> m_track_statuses;
        std::string m__unnamed16;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        event_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~event_t();
        uint64_t tick() const { return m_tick; }
        void set_tick(uint64_t _v) { m__dirty = true; m_tick = std::move(_v); }
        uint64_t duration_bits_1() const { return m_duration_bits_1; }
        void set_duration_bits_1(uint64_t _v) { m__dirty = true; n_duration_bits_1 = false; m_duration_bits_1 = std::move(_v); }
        uint64_t track() const { return m_track; }
        void set_track(uint64_t _v) { m__dirty = true; n_track = false; m_track = std::move(_v); }
        uint64_t duration_bits_2() const { return m_duration_bits_2; }
        void set_duration_bits_2(uint64_t _v) { m__dirty = true; n_duration_bits_2 = false; m_duration_bits_2 = std::move(_v); }
        uint8_t id() const { return m_id; }
        void set_id(uint8_t _v) { m__dirty = true; n_id = false; m_id = std::move(_v); }
        uint64_t terminator_pad() const { return m_terminator_pad; }
        void set_terminator_pad(uint64_t _v) { m__dirty = true; n_terminator_pad = false; m_terminator_pad = std::move(_v); }
        note_event_t* note_event() const { return m_note_event.get(); }
        void set_note_event(std::unique_ptr<note_event_t> _v) { m__dirty = true; m_note_event = std::move(_v); }
        pitch_bend_event_t* pitch_bend() const { return m_pitch_bend.get(); }
        void set_pitch_bend(std::unique_ptr<pitch_bend_event_t> _v) { m__dirty = true; m_pitch_bend = std::move(_v); }
        control_change_event_t* control_change() const { return m_control_change.get(); }
        void set_control_change(std::unique_ptr<control_change_event_t> _v) { m__dirty = true; m_control_change = std::move(_v); }
        program_change_event_t* program_change() const { return m_program_change.get(); }
        void set_program_change(std::unique_ptr<program_change_event_t> _v) { m__dirty = true; m_program_change = std::move(_v); }
        ch_pressure_event_t* ch_pressure() const { return m_ch_pressure.get(); }
        void set_ch_pressure(std::unique_ptr<ch_pressure_event_t> _v) { m__dirty = true; m_ch_pressure = std::move(_v); }
        poly_pressure_event_t* poly_pressure() const { return m_poly_pressure.get(); }
        void set_poly_pressure(std::unique_ptr<poly_pressure_event_t> _v) { m__dirty = true; m_poly_pressure = std::move(_v); }
        exclusive_event_t* exclusive() const { return m_exclusive.get(); }
        void set_exclusive(std::unique_ptr<exclusive_event_t> _v) { m__dirty = true; m_exclusive = std::move(_v); }
        std::string terminator() const { return m_terminator; }
        void set_terminator(std::string _v) { m__dirty = true; n_terminator = false; m_terminator = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::sequence_body_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint64_t m_tick;
        uint64_t m_duration_bits_1;
        bool n_duration_bits_1;

    public:
        bool _is_null_duration_bits_1() { duration_bits_1(); return n_duration_bits_1; };

    private:
        uint64_t m_track;
        bool n_track;

    public:
        bool _is_null_track() { track(); return n_track; };

    private:
        uint64_t m_duration_bits_2;
        bool n_duration_bits_2;

    public:
        bool _is_null_duration_bits_2() { duration_bits_2(); return n_duration_bits_2; };

    private:
        uint8_t m_id;
        bool n_id;

    public:
        bool _is_null_id() { id(); return n_id; };

    private:
        uint64_t m_terminator_pad;
        bool n_terminator_pad;

    public:
        bool _is_null_terminator_pad() { terminator_pad(); return n_terminator_pad; };

    private:
        std::unique_ptr<note_event_t> m_note_event;

    public:
        bool _is_null_note_event() { return !note_event(); };

    private:
        std::unique_ptr<pitch_bend_event_t> m_pitch_bend;

    public:
        bool _is_null_pitch_bend() { return !pitch_bend(); };

    private:
        std::unique_ptr<control_change_event_t> m_control_change;

    public:
        bool _is_null_control_change() { return !control_change(); };

    private:
        std::unique_ptr<program_change_event_t> m_program_change;

    public:
        bool _is_null_program_change() { return !program_change(); };

    private:
        std::unique_ptr<ch_pressure_event_t> m_ch_pressure;

    public:
        bool _is_null_ch_pressure() { return !ch_pressure(); };

    private:
        std::unique_ptr<poly_pressure_event_t> m_poly_pressure;

    public:
        bool _is_null_poly_pressure() { return !poly_pressure(); };

    private:
        std::unique_ptr<exclusive_event_t> m_exclusive;

    public:
        bool _is_null_exclusive() { return !exclusive(); };

    private:
        std::string m_terminator;
        bool n_terminator;

    public:
        bool _is_null_terminator() { terminator(); return n_terminator; };

    private:
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::sequence_body_t* m__parent;
    };

    class exclusive_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        exclusive_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~exclusive_event_t();
        std::string _unnamed0() const { return m__unnamed0; }
        void set__unnamed0(std::string _v) { m__dirty = true; m__unnamed0 = std::move(_v); }
        std::string bytes() const { return m_bytes; }
        void set_bytes(std::string _v) { m__dirty = true; m_bytes = std::move(_v); }
        mixer_event_t* mixer() const { return m_mixer.get(); }
        void set_mixer(std::unique_ptr<mixer_event_t> _v) { m__dirty = true; m_mixer = std::move(_v); }
        std::string _unnamed3() const { return m__unnamed3; }
        void set__unnamed3(std::string _v) { m__dirty = true; m__unnamed3 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m__unnamed0;
        std::string m_bytes;
        std::unique_ptr<mixer_event_t> m_mixer;

    public:
        bool _is_null_mixer() { return !mixer(); };

    private:
        std::string m__unnamed3;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::event_t* m__parent;
    };

    class location_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        location_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~location_t();
        std::string display_value();
        void _invalidate_display_value() { f_display_value = false; }
        uint16_t bar() const { return m_bar; }
        void set_bar(uint16_t _v) { m__dirty = true; f_display_value = false; m_bar = std::move(_v); }
        uint8_t beat() const { return m_beat; }
        void set_beat(uint8_t _v) { m__dirty = true; f_display_value = false; m_beat = std::move(_v); }
        uint8_t clock() const { return m_clock; }
        void set_clock(uint8_t _v) { m__dirty = true; f_display_value = false; m_clock = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; f_display_value = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; f_display_value = false; m__parent = std::move(_v); }

    private:
        bool f_display_value;
        std::string m_display_value;
        uint16_t m_bar;
        uint8_t m_beat;
        uint8_t m_clock;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class midi_input_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        midi_input_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~midi_input_t();
        midi_input_receive_channel_t receive_channel() const { return m_receive_channel; }
        void set_receive_channel(midi_input_receive_channel_t _v) { m__dirty = true; m_receive_channel = std::move(_v); }
        bool sustain_pedal_to_duration() const { return m_sustain_pedal_to_duration; }
        void set_sustain_pedal_to_duration(bool _v) { m__dirty = true; m_sustain_pedal_to_duration = std::move(_v); }
        uint64_t _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(uint64_t _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        bool filter_enabled() const { return m_filter_enabled; }
        void set_filter_enabled(bool _v) { m__dirty = true; m_filter_enabled = std::move(_v); }
        uint64_t _unnamed4() const { return m__unnamed4; }
        void set__unnamed4(uint64_t _v) { m__dirty = true; m__unnamed4 = std::move(_v); }
        midi_input_filter_type_t filter_type() const { return m_filter_type; }
        void set_filter_type(midi_input_filter_type_t _v) { m__dirty = true; m_filter_type = std::move(_v); }
        bool multi_rec_enabled() const { return m_multi_rec_enabled; }
        void set_multi_rec_enabled(bool _v) { m__dirty = true; m_multi_rec_enabled = std::move(_v); }
        uint64_t _unnamed7() const { return m__unnamed7; }
        void set__unnamed7(uint64_t _v) { m__dirty = true; m__unnamed7 = std::move(_v); }
        std::vector<uint8_t>* multi_rec_destination_tracks() const { return m_multi_rec_destination_tracks.get(); }
        void set_multi_rec_destination_tracks(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_multi_rec_destination_tracks = std::move(_v); }
        bool note_pass_enabled() const { return m_note_pass_enabled; }
        void set_note_pass_enabled(bool _v) { m__dirty = true; m_note_pass_enabled = std::move(_v); }
        uint64_t _unnamed10() const { return m__unnamed10; }
        void set__unnamed10(uint64_t _v) { m__dirty = true; m__unnamed10 = std::move(_v); }
        bool pitch_bend_pass_enabled() const { return m_pitch_bend_pass_enabled; }
        void set_pitch_bend_pass_enabled(bool _v) { m__dirty = true; m_pitch_bend_pass_enabled = std::move(_v); }
        uint64_t _unnamed12() const { return m__unnamed12; }
        void set__unnamed12(uint64_t _v) { m__dirty = true; m__unnamed12 = std::move(_v); }
        bool pgm_change_pass_enabled() const { return m_pgm_change_pass_enabled; }
        void set_pgm_change_pass_enabled(bool _v) { m__dirty = true; m_pgm_change_pass_enabled = std::move(_v); }
        uint64_t _unnamed14() const { return m__unnamed14; }
        void set__unnamed14(uint64_t _v) { m__dirty = true; m__unnamed14 = std::move(_v); }
        bool ch_pressure_pass_enabled() const { return m_ch_pressure_pass_enabled; }
        void set_ch_pressure_pass_enabled(bool _v) { m__dirty = true; m_ch_pressure_pass_enabled = std::move(_v); }
        uint64_t _unnamed16() const { return m__unnamed16; }
        void set__unnamed16(uint64_t _v) { m__dirty = true; m__unnamed16 = std::move(_v); }
        bool poly_pressure_pass_enabled() const { return m_poly_pressure_pass_enabled; }
        void set_poly_pressure_pass_enabled(bool _v) { m__dirty = true; m_poly_pressure_pass_enabled = std::move(_v); }
        uint64_t _unnamed18() const { return m__unnamed18; }
        void set__unnamed18(uint64_t _v) { m__dirty = true; m__unnamed18 = std::move(_v); }
        bool exclusive_pass_enabled() const { return m_exclusive_pass_enabled; }
        void set_exclusive_pass_enabled(bool _v) { m__dirty = true; m_exclusive_pass_enabled = std::move(_v); }
        uint64_t _unnamed20() const { return m__unnamed20; }
        void set__unnamed20(uint64_t _v) { m__dirty = true; m__unnamed20 = std::move(_v); }
        std::vector<bool>* cc_pass_enabled() const { return m_cc_pass_enabled.get(); }
        void set_cc_pass_enabled(std::unique_ptr<std::vector<bool>> _v) { m__dirty = true; m_cc_pass_enabled = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        midi_input_receive_channel_t m_receive_channel;
        bool m_sustain_pedal_to_duration;
        uint64_t m__unnamed2;
        bool m_filter_enabled;
        uint64_t m__unnamed4;
        midi_input_filter_type_t m_filter_type;
        bool m_multi_rec_enabled;
        uint64_t m__unnamed7;
        std::unique_ptr<std::vector<uint8_t>> m_multi_rec_destination_tracks;
        bool m_note_pass_enabled;
        uint64_t m__unnamed10;
        bool m_pitch_bend_pass_enabled;
        uint64_t m__unnamed12;
        bool m_pgm_change_pass_enabled;
        uint64_t m__unnamed14;
        bool m_ch_pressure_pass_enabled;
        uint64_t m__unnamed16;
        bool m_poly_pressure_pass_enabled;
        uint64_t m__unnamed18;
        bool m_exclusive_pass_enabled;
        uint64_t m__unnamed20;
        std::unique_ptr<std::vector<bool>> m_cc_pass_enabled;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class midi_output_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        midi_output_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~midi_output_t();
        soft_thru_mode_t soft_thru_mode() const { return m_soft_thru_mode; }
        void set_soft_thru_mode(soft_thru_mode_t _v) { m__dirty = true; m_soft_thru_mode = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        soft_thru_mode_t m_soft_thru_mode;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class midi_switch_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        midi_switch_t(kaitai::kstream* p__io, mpc2000xl_all_t::misc_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~midi_switch_t();

        /**
         * 0xFF is used for an unassigned footswitch controller.
         */
        uint8_t controller() const { return m_controller; }
        void set_controller(uint8_t _v) { m__dirty = true; m_controller = std::move(_v); }
        midi_switch_function_t function() const { return m_function; }
        void set_function(midi_switch_function_t _v) { m__dirty = true; m_function = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::misc_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_controller;
        midi_switch_function_t m_function;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::misc_t* m__parent;
    };

    class midi_sync_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        midi_sync_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~midi_sync_t();
        midi_sync_mode_t in_mode() const { return m_in_mode; }
        void set_in_mode(midi_sync_mode_t _v) { m__dirty = true; m_in_mode = std::move(_v); }
        midi_sync_mode_t out_mode() const { return m_out_mode; }
        void set_out_mode(midi_sync_mode_t _v) { m__dirty = true; m_out_mode = std::move(_v); }
        uint8_t shift_early() const { return m_shift_early; }
        void set_shift_early(uint8_t _v) { m__dirty = true; m_shift_early = std::move(_v); }
        bool send_mmc_enabled() const { return m_send_mmc_enabled; }
        void set_send_mmc_enabled(bool _v) { m__dirty = true; m_send_mmc_enabled = std::move(_v); }
        uint64_t _unnamed4() const { return m__unnamed4; }
        void set__unnamed4(uint64_t _v) { m__dirty = true; m__unnamed4 = std::move(_v); }
        frame_rate_t frame_rate() const { return m_frame_rate; }
        void set_frame_rate(frame_rate_t _v) { m__dirty = true; m_frame_rate = std::move(_v); }
        uint8_t input() const { return m_input; }
        void set_input(uint8_t _v) { m__dirty = true; m_input = std::move(_v); }
        midi_sync_output_t output() const { return m_output; }
        void set_output(midi_sync_output_t _v) { m__dirty = true; m_output = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        midi_sync_mode_t m_in_mode;
        midi_sync_mode_t m_out_mode;
        uint8_t m_shift_early;
        bool m_send_mmc_enabled;
        uint64_t m__unnamed4;
        frame_rate_t m_frame_rate;
        uint8_t m_input;
        midi_sync_output_t m_output;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class misc_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        misc_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~misc_t();
        tap_averaging_t tap_averaging() const { return m_tap_averaging; }
        void set_tap_averaging(tap_averaging_t _v) { m__dirty = true; m_tap_averaging = std::move(_v); }
        bool midi_sync_in_receive_mmc_enabled() const { return m_midi_sync_in_receive_mmc_enabled; }
        void set_midi_sync_in_receive_mmc_enabled(bool _v) { m__dirty = true; m_midi_sync_in_receive_mmc_enabled = std::move(_v); }
        std::vector<std::unique_ptr<midi_switch_t>>* midi_switch() const { return m_midi_switch.get(); }
        void set_midi_switch(std::unique_ptr<std::vector<std::unique_ptr<midi_switch_t>>> _v) { m__dirty = true; m_midi_switch = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        tap_averaging_t m_tap_averaging;
        bool m_midi_sync_in_receive_mmc_enabled;
        std::unique_ptr<std::vector<std::unique_ptr<midi_switch_t>>> m_midi_switch;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class mixer_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        mixer_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::exclusive_event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~mixer_event_t();
        std::string _unnamed0() const { return m__unnamed0; }
        void set__unnamed0(std::string _v) { m__dirty = true; m__unnamed0 = std::move(_v); }
        mixer_event_param_t param() const { return m_param; }
        void set_param(mixer_event_param_t _v) { m__dirty = true; m_param = std::move(_v); }
        uint8_t pad_index() const { return m_pad_index; }
        void set_pad_index(uint8_t _v) { m__dirty = true; m_pad_index = std::move(_v); }
        uint8_t value() const { return m_value; }
        void set_value(uint8_t _v) { m__dirty = true; m_value = std::move(_v); }
        std::string _unnamed4() const { return m__unnamed4; }
        void set__unnamed4(std::string _v) { m__dirty = true; m__unnamed4 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::exclusive_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m__unnamed0;
        mixer_event_param_t m_param;
        uint8_t m_pad_index;
        uint8_t m_value;
        std::string m__unnamed4;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::exclusive_event_t* m__parent;
    };

    class note_event_t : public kaitai::kstruct {

    public:

        enum note_variation_type_t {
            NOTE_VARIATION_TYPE_TUNE = 0,
            NOTE_VARIATION_TYPE_DECAY = 1,
            NOTE_VARIATION_TYPE_ATTACK = 2,
            NOTE_VARIATION_TYPE_FILTER = 3
        };
        static bool _is_defined_note_variation_type_t(note_variation_type_t v);

    private:
        static const std::set<note_variation_type_t> _values_note_variation_type_t;

    public:

    private:
        bool m__dirty;

    public:

        note_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~note_event_t();
        int32_t duration();
        void _invalidate_duration() { f_duration = false; }
        uint8_t note();
        void _invalidate_note() { f_note = false; }
        mpc2000xl_all_t::note_event_t::note_variation_type_t variation_type();
        void _invalidate_variation_type() { f_variation_type = false; }
        uint8_t duration_bits_3() const { return m_duration_bits_3; }
        void set_duration_bits_3(uint8_t _v) { m__dirty = true; f_duration = false; f_note = false; f_variation_type = false; m_duration_bits_3 = std::move(_v); }
        uint64_t velocity() const { return m_velocity; }
        void set_velocity(uint64_t _v) { m__dirty = true; f_duration = false; f_note = false; f_variation_type = false; m_velocity = std::move(_v); }
        bool variation_type_bit_1() const { return m_variation_type_bit_1; }
        void set_variation_type_bit_1(bool _v) { m__dirty = true; f_duration = false; f_note = false; f_variation_type = false; m_variation_type_bit_1 = std::move(_v); }
        uint64_t variation_value() const { return m_variation_value; }
        void set_variation_value(uint64_t _v) { m__dirty = true; f_duration = false; f_note = false; f_variation_type = false; m_variation_value = std::move(_v); }
        bool variation_type_bit_2() const { return m_variation_type_bit_2; }
        void set_variation_type_bit_2(bool _v) { m__dirty = true; f_duration = false; f_note = false; f_variation_type = false; m_variation_type_bit_2 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; f_duration = false; f_note = false; f_variation_type = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::event_t* _v) { m__dirty = true; f_duration = false; f_note = false; f_variation_type = false; m__parent = std::move(_v); }

    private:
        bool f_duration;
        int32_t m_duration;
        bool f_note;
        uint8_t m_note;
        bool f_variation_type;
        mpc2000xl_all_t::note_event_t::note_variation_type_t m_variation_type;
        uint8_t m_duration_bits_3;
        uint64_t m_velocity;
        bool m_variation_type_bit_1;
        uint64_t m_variation_value;
        bool m_variation_type_bit_2;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::event_t* m__parent;
    };

    class pitch_bend_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        pitch_bend_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~pitch_bend_event_t();
        int32_t corrected_amount();
        void _invalidate_corrected_amount() { f_corrected_amount = false; }
        uint64_t amount_bits_1() const { return m_amount_bits_1; }
        void set_amount_bits_1(uint64_t _v) { m__dirty = true; f_corrected_amount = false; m_amount_bits_1 = std::move(_v); }
        uint64_t amount_bits_2() const { return m_amount_bits_2; }
        void set_amount_bits_2(uint64_t _v) { m__dirty = true; f_corrected_amount = false; m_amount_bits_2 = std::move(_v); }
        std::string _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(std::string _v) { m__dirty = true; f_corrected_amount = false; m__unnamed2 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; f_corrected_amount = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::event_t* _v) { m__dirty = true; f_corrected_amount = false; m__parent = std::move(_v); }

    private:
        bool f_corrected_amount;
        int32_t m_corrected_amount;
        uint64_t m_amount_bits_1;
        uint64_t m_amount_bits_2;
        std::string m__unnamed2;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::event_t* m__parent;
    };

    class poly_pressure_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        poly_pressure_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~poly_pressure_event_t();
        uint8_t note() const { return m_note; }
        void set_note(uint8_t _v) { m__dirty = true; m_note = std::move(_v); }
        uint8_t pressure() const { return m_pressure; }
        void set_pressure(uint8_t _v) { m__dirty = true; m_pressure = std::move(_v); }
        std::string _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(std::string _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_note;
        uint8_t m_pressure;
        std::string m__unnamed2;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::event_t* m__parent;
    };

    class program_change_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        program_change_event_t(kaitai::kstream* p__io, mpc2000xl_all_t::event_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~program_change_event_t();
        uint8_t program() const { return m_program; }
        void set_program(uint8_t _v) { m__dirty = true; m_program = std::move(_v); }
        std::string _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(std::string _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_program;
        std::string m__unnamed1;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::event_t* m__parent;
    };

    class sequence_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sequence_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequence_t();
        std::string name();
        void _invalidate_name() { f_name = false; }
        std::string name_part_1() const { return m_name_part_1; }
        void set_name_part_1(std::string _v) { m__dirty = true; f_name = false; m_name_part_1 = std::move(_v); }
        std::string name_part_2() const { return m_name_part_2; }
        void set_name_part_2(std::string _v) { m__dirty = true; n_name_part_2 = false; f_name = false; m_name_part_2 = std::move(_v); }
        sequence_body_t* body() const { return m_body.get(); }
        void set_body(std::unique_ptr<sequence_body_t> _v) { m__dirty = true; f_name = false; m_body = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; f_name = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; f_name = false; m__parent = std::move(_v); }

    private:
        bool f_name;
        std::string m_name;
        bool n_name;

    public:
        bool _is_null_name() { name(); return n_name; };

    private:
        std::string m_name_part_1;
        std::string m_name_part_2;
        bool n_name_part_2;

    public:
        bool _is_null_name_part_2() { name_part_2(); return n_name_part_2; };

    private:
        std::unique_ptr<sequence_body_t> m_body;

    public:
        bool _is_null_body() { return !body(); };

    private:
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class sequence_body_t : public kaitai::kstruct {

    public:
        class start_time_t;

    private:
        bool m__dirty;

    public:

        sequence_body_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequence_body_t();

        class start_time_t : public kaitai::kstruct {

        public:

        private:
            bool m__dirty;

        public:

            start_time_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
            void _read();
            void _fetch_instances();
            void _write();
            void _check();
            ~start_time_t();
            uint8_t hours() const { return m_hours; }
            void set_hours(uint8_t _v) { m__dirty = true; m_hours = std::move(_v); }
            uint8_t minutes() const { return m_minutes; }
            void set_minutes(uint8_t _v) { m__dirty = true; m_minutes = std::move(_v); }
            uint8_t seconds() const { return m_seconds; }
            void set_seconds(uint8_t _v) { m__dirty = true; m_seconds = std::move(_v); }
            uint8_t frames() const { return m_frames; }
            void set_frames(uint8_t _v) { m__dirty = true; m_frames = std::move(_v); }
            uint8_t frame_decimals() const { return m_frame_decimals; }
            void set_frame_decimals(uint8_t _v) { m__dirty = true; m_frame_decimals = std::move(_v); }
            mpc2000xl_all_t* _root() const { return m__root; }
            void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
            kaitai::kstruct* _parent() const { return m__parent; }
            void set__parent(mpc2000xl_all_t::sequence_body_t* _v) { m__dirty = true; m__parent = std::move(_v); }

        private:
            uint8_t m_hours;
            uint8_t m_minutes;
            uint8_t m_seconds;
            uint8_t m_frames;
            uint8_t m_frame_decimals;
            mpc2000xl_all_t* m__root;
            mpc2000xl_all_t::sequence_body_t* m__parent;
        };

    public:
        sequence_is_used_t is_used() const { return m_is_used; }
        void set_is_used(sequence_is_used_t _v) { m__dirty = true; m_is_used = std::move(_v); }

        /**
         * Warning! 1-based. You'll need this to put the sequence in the right slot
         */
        uint8_t index() const { return m_index; }
        void set_index(uint8_t _v) { m__dirty = true; m_index = std::move(_v); }
        std::string _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(std::string _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        uint16_t bar_count() const { return m_bar_count; }
        void set_bar_count(uint16_t _v) { m__dirty = true; m_bar_count = std::move(_v); }
        uint32_t last_tick() const { return m_last_tick; }
        void set_last_tick(uint32_t _v) { m__dirty = true; m_last_tick = std::move(_v); }
        std::string _unnamed5() const { return m__unnamed5; }
        void set__unnamed5(std::string _v) { m__dirty = true; m__unnamed5 = std::move(_v); }
        uint16_t loop_start_bar_index() const { return m_loop_start_bar_index; }
        void set_loop_start_bar_index(uint16_t _v) { m__dirty = true; m_loop_start_bar_index = std::move(_v); }
        uint16_t loop_end_bar_index() const { return m_loop_end_bar_index; }
        void set_loop_end_bar_index(uint16_t _v) { m__dirty = true; m_loop_end_bar_index = std::move(_v); }
        bool loop_enabled() const { return m_loop_enabled; }
        void set_loop_enabled(bool _v) { m__dirty = true; m_loop_enabled = std::move(_v); }
        uint64_t _unnamed9() const { return m__unnamed9; }
        void set__unnamed9(uint64_t _v) { m__dirty = true; m__unnamed9 = std::move(_v); }
        start_time_t* start_time() const { return m_start_time.get(); }
        void set_start_time(std::unique_ptr<start_time_t> _v) { m__dirty = true; m_start_time = std::move(_v); }
        std::string _unnamed11() const { return m__unnamed11; }
        void set__unnamed11(std::string _v) { m__dirty = true; m__unnamed11 = std::move(_v); }
        uint32_t last_tick2() const { return m_last_tick2; }
        void set_last_tick2(uint32_t _v) { m__dirty = true; m_last_tick2 = std::move(_v); }
        std::string _unnamed13() const { return m__unnamed13; }
        void set__unnamed13(std::string _v) { m__dirty = true; m__unnamed13 = std::move(_v); }
        std::vector<std::string>* device_names() const { return m_device_names.get(); }
        void set_device_names(std::unique_ptr<std::vector<std::string>> _v) { m__dirty = true; m_device_names = std::move(_v); }
        tracks_t* tracks() const { return m_tracks.get(); }
        void set_tracks(std::unique_ptr<tracks_t> _v) { m__dirty = true; m_tracks = std::move(_v); }
        std::string _unnamed16() const { return m__unnamed16; }
        void set__unnamed16(std::string _v) { m__dirty = true; m__unnamed16 = std::move(_v); }
        std::vector<std::unique_ptr<bar_t>>* bars() const { return m_bars.get(); }
        void set_bars(std::unique_ptr<std::vector<std::unique_ptr<bar_t>>> _v) { m__dirty = true; m_bars = std::move(_v); }
        std::string _unnamed18() const { return m__unnamed18; }
        void set__unnamed18(std::string _v) { m__dirty = true; m__unnamed18 = std::move(_v); }
        std::string _unnamed19() const { return m__unnamed19; }
        void set__unnamed19(std::string _v) { m__dirty = true; m__unnamed19 = std::move(_v); }
        std::vector<std::unique_ptr<event_t>>* events() const { return m_events.get(); }
        void set_events(std::unique_ptr<std::vector<std::unique_ptr<event_t>>> _v) { m__dirty = true; m_events = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::sequence_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        sequence_is_used_t m_is_used;
        uint8_t m_index;
        std::string m__unnamed2;
        uint16_t m_bar_count;
        uint32_t m_last_tick;
        std::string m__unnamed5;
        uint16_t m_loop_start_bar_index;
        uint16_t m_loop_end_bar_index;
        bool m_loop_enabled;
        uint64_t m__unnamed9;
        std::unique_ptr<start_time_t> m_start_time;
        std::string m__unnamed11;
        uint32_t m_last_tick2;
        std::string m__unnamed13;
        std::unique_ptr<std::vector<std::string>> m_device_names;
        std::unique_ptr<tracks_t> m_tracks;
        std::string m__unnamed16;
        std::unique_ptr<std::vector<std::unique_ptr<bar_t>>> m_bars;
        std::string m__unnamed18;
        std::string m__unnamed19;
        std::unique_ptr<std::vector<std::unique_ptr<event_t>>> m_events;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::sequence_t* m__parent;
    };

    class sequence_meta_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sequence_meta_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequence_meta_t();
        bool is_used();
        void _invalidate_is_used() { f_is_used = false; }
        std::string name() const { return m_name; }
        void set_name(std::string _v) { m__dirty = true; f_is_used = false; m_name = std::move(_v); }
        uint16_t last_event_index() const { return m_last_event_index; }
        void set_last_event_index(uint16_t _v) { m__dirty = true; f_is_used = false; m_last_event_index = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; f_is_used = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; f_is_used = false; m__parent = std::move(_v); }

    private:
        bool f_is_used;
        bool m_is_used;
        std::string m_name;
        uint16_t m_last_event_index;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class sequencer_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sequencer_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequencer_t();
        uint8_t active_sequence() const { return m_active_sequence; }
        void set_active_sequence(uint8_t _v) { m__dirty = true; m_active_sequence = std::move(_v); }
        std::string _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(std::string _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        uint8_t active_track() const { return m_active_track; }
        void set_active_track(uint8_t _v) { m__dirty = true; m_active_track = std::move(_v); }
        std::string _unnamed3() const { return m__unnamed3; }
        void set__unnamed3(std::string _v) { m__dirty = true; m__unnamed3 = std::move(_v); }
        uint16_t master_tempo() const { return m_master_tempo; }
        void set_master_tempo(uint16_t _v) { m__dirty = true; m_master_tempo = std::move(_v); }
        bool tempo_source_is_sequence() const { return m_tempo_source_is_sequence; }
        void set_tempo_source_is_sequence(bool _v) { m__dirty = true; m_tempo_source_is_sequence = std::move(_v); }
        uint64_t _unnamed6() const { return m__unnamed6; }
        void set__unnamed6(uint64_t _v) { m__dirty = true; m__unnamed6 = std::move(_v); }
        timing_correct_t timing_correct() const { return m_timing_correct; }
        void set_timing_correct(timing_correct_t _v) { m__dirty = true; m_timing_correct = std::move(_v); }
        time_display_style_t time_display_style() const { return m_time_display_style; }
        void set_time_display_style(time_display_style_t _v) { m__dirty = true; m_time_display_style = std::move(_v); }
        uint64_t _unnamed9() const { return m__unnamed9; }
        void set__unnamed9(uint64_t _v) { m__dirty = true; m__unnamed9 = std::move(_v); }
        bool second_sequence_enabled() const { return m_second_sequence_enabled; }
        void set_second_sequence_enabled(bool _v) { m__dirty = true; m_second_sequence_enabled = std::move(_v); }
        uint64_t _unnamed11() const { return m__unnamed11; }
        void set__unnamed11(uint64_t _v) { m__dirty = true; m__unnamed11 = std::move(_v); }
        uint8_t sequence_sequence_index() const { return m_sequence_sequence_index; }
        void set_sequence_sequence_index(uint8_t _v) { m__dirty = true; m_sequence_sequence_index = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_active_sequence;
        std::string m__unnamed1;
        uint8_t m_active_track;
        std::string m__unnamed3;
        uint16_t m_master_tempo;
        bool m_tempo_source_is_sequence;
        uint64_t m__unnamed6;
        timing_correct_t m_timing_correct;
        time_display_style_t m_time_display_style;
        uint64_t m__unnamed9;
        bool m_second_sequence_enabled;
        uint64_t m__unnamed11;
        uint8_t m_sequence_sequence_index;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class song_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        song_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~song_t();
        std::string name() const { return m_name; }
        void set_name(std::string _v) { m__dirty = true; m_name = std::move(_v); }
        std::vector<std::unique_ptr<song_step_t>>* steps() const { return m_steps.get(); }
        void set_steps(std::unique_ptr<std::vector<std::unique_ptr<song_step_t>>> _v) { m__dirty = true; m_steps = std::move(_v); }
        std::string _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(std::string _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        bool is_used() const { return m_is_used; }
        void set_is_used(bool _v) { m__dirty = true; m_is_used = std::move(_v); }
        uint8_t loop_first_step() const { return m_loop_first_step; }
        void set_loop_first_step(uint8_t _v) { m__dirty = true; m_loop_first_step = std::move(_v); }
        uint8_t loop_last_step() const { return m_loop_last_step; }
        void set_loop_last_step(uint8_t _v) { m__dirty = true; m_loop_last_step = std::move(_v); }
        bool is_loop_enabled() const { return m_is_loop_enabled; }
        void set_is_loop_enabled(bool _v) { m__dirty = true; m_is_loop_enabled = std::move(_v); }
        std::string _unnamed7() const { return m__unnamed7; }
        void set__unnamed7(std::string _v) { m__dirty = true; m__unnamed7 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m_name;
        std::unique_ptr<std::vector<std::unique_ptr<song_step_t>>> m_steps;
        std::string m__unnamed2;
        bool m_is_used;
        uint8_t m_loop_first_step;
        uint8_t m_loop_last_step;
        bool m_is_loop_enabled;
        std::string m__unnamed7;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class song_global_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        song_global_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~song_global_t();
        std::string default_song_name() const { return m_default_song_name; }
        void set_default_song_name(std::string _v) { m__dirty = true; m_default_song_name = std::move(_v); }
        bool ignore_tempo_change_events_in_sequence() const { return m_ignore_tempo_change_events_in_sequence; }
        void set_ignore_tempo_change_events_in_sequence(bool _v) { m__dirty = true; m_ignore_tempo_change_events_in_sequence = std::move(_v); }
        uint64_t _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(uint64_t _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m_default_song_name;
        bool m_ignore_tempo_change_events_in_sequence;
        uint64_t m__unnamed2;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class song_step_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        song_step_t(kaitai::kstream* p__io, mpc2000xl_all_t::song_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~song_step_t();
        uint8_t sequence_index() const { return m_sequence_index; }
        void set_sequence_index(uint8_t _v) { m__dirty = true; m_sequence_index = std::move(_v); }
        uint8_t repeat_count() const { return m_repeat_count; }
        void set_repeat_count(uint8_t _v) { m__dirty = true; m_repeat_count = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::song_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_sequence_index;
        uint8_t m_repeat_count;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::song_t* m__parent;
    };

    class step_edit_options_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        step_edit_options_t(kaitai::kstream* p__io, mpc2000xl_all_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~step_edit_options_t();
        bool auto_step_increment() const { return m_auto_step_increment; }
        void set_auto_step_increment(bool _v) { m__dirty = true; m_auto_step_increment = std::move(_v); }
        duration_of_recorded_notes_t duration_of_recorded_notes() const { return m_duration_of_recorded_notes; }
        void set_duration_of_recorded_notes(duration_of_recorded_notes_t _v) { m__dirty = true; m_duration_of_recorded_notes = std::move(_v); }
        uint8_t tc_value_percentage() const { return m_tc_value_percentage; }
        void set_tc_value_percentage(uint8_t _v) { m__dirty = true; m_tc_value_percentage = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        bool m_auto_step_increment;
        duration_of_recorded_notes_t m_duration_of_recorded_notes;
        uint8_t m_tc_value_percentage;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t* m__parent;
    };

    class track_status_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        track_status_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~track_status_t();
        unused_used_t unused_or_used() const { return m_unused_or_used; }
        void set_unused_or_used(unused_used_t _v) { m__dirty = true; m_unused_or_used = std::move(_v); }
        off_on_t off_or_on() const { return m_off_or_on; }
        void set_off_or_on(off_on_t _v) { m__dirty = true; m_off_or_on = std::move(_v); }
        off_on_t transmit_program_changes() const { return m_transmit_program_changes; }
        void set_transmit_program_changes(off_on_t _v) { m__dirty = true; m_transmit_program_changes = std::move(_v); }
        uint64_t _unnamed3() const { return m__unnamed3; }
        void set__unnamed3(uint64_t _v) { m__dirty = true; m__unnamed3 = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        unused_used_t m_unused_or_used;
        off_on_t m_off_or_on;
        off_on_t m_transmit_program_changes;
        uint64_t m__unnamed3;
        mpc2000xl_all_t* m__root;
        kaitai::kstruct* m__parent;
    };

    class tracks_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        tracks_t(kaitai::kstream* p__io, mpc2000xl_all_t::sequence_body_t* p__parent = nullptr, mpc2000xl_all_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~tracks_t();
        std::vector<std::string>* names() const { return m_names.get(); }
        void set_names(std::unique_ptr<std::vector<std::string>> _v) { m__dirty = true; m_names = std::move(_v); }
        std::vector<uint8_t>* device() const { return m_device.get(); }
        void set_device(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_device = std::move(_v); }
        std::vector<bus_t>* bus() const { return m_bus.get(); }
        void set_bus(std::unique_ptr<std::vector<bus_t>> _v) { m__dirty = true; m_bus = std::move(_v); }
        std::vector<uint8_t>* program_change() const { return m_program_change.get(); }
        void set_program_change(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_program_change = std::move(_v); }
        std::vector<uint8_t>* velocity_ratio() const { return m_velocity_ratio.get(); }
        void set_velocity_ratio(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_velocity_ratio = std::move(_v); }
        std::vector<std::unique_ptr<track_status_t>>* status() const { return m_status.get(); }
        void set_status(std::unique_ptr<std::vector<std::unique_ptr<track_status_t>>> _v) { m__dirty = true; m_status = std::move(_v); }
        std::string unknown() const { return m_unknown; }
        void set_unknown(std::string _v) { m__dirty = true; m_unknown = std::move(_v); }
        mpc2000xl_all_t* _root() const { return m__root; }
        void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_all_t::sequence_body_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::unique_ptr<std::vector<std::string>> m_names;
        std::unique_ptr<std::vector<uint8_t>> m_device;
        std::unique_ptr<std::vector<bus_t>> m_bus;
        std::unique_ptr<std::vector<uint8_t>> m_program_change;
        std::unique_ptr<std::vector<uint8_t>> m_velocity_ratio;
        std::unique_ptr<std::vector<std::unique_ptr<track_status_t>>> m_status;
        std::string m_unknown;
        mpc2000xl_all_t* m__root;
        mpc2000xl_all_t::sequence_body_t* m__parent;
    };

public:
    std::string magic() const { return m_magic; }
    void set_magic(std::string _v) { m__dirty = true; m_magic = std::move(_v); }
    defaults_t* defaults() const { return m_defaults.get(); }
    void set_defaults(std::unique_ptr<defaults_t> _v) { m__dirty = true; m_defaults = std::move(_v); }
    sequencer_t* sequencer() const { return m_sequencer.get(); }
    void set_sequencer(std::unique_ptr<sequencer_t> _v) { m__dirty = true; m_sequencer = std::move(_v); }
    std::string _unnamed3() const { return m__unnamed3; }
    void set__unnamed3(std::string _v) { m__dirty = true; m__unnamed3 = std::move(_v); }
    count_t* count() const { return m_count.get(); }
    void set_count(std::unique_ptr<count_t> _v) { m__dirty = true; m_count = std::move(_v); }
    midi_output_t* midi_output() const { return m_midi_output.get(); }
    void set_midi_output(std::unique_ptr<midi_output_t> _v) { m__dirty = true; m_midi_output = std::move(_v); }
    midi_input_t* midi_input() const { return m_midi_input.get(); }
    void set_midi_input(std::unique_ptr<midi_input_t> _v) { m__dirty = true; m_midi_input = std::move(_v); }
    midi_sync_t* midi_sync() const { return m_midi_sync.get(); }
    void set_midi_sync(std::unique_ptr<midi_sync_t> _v) { m__dirty = true; m_midi_sync = std::move(_v); }
    song_global_t* song_global() const { return m_song_global.get(); }
    void set_song_global(std::unique_ptr<song_global_t> _v) { m__dirty = true; m_song_global = std::move(_v); }
    std::string _unnamed9() const { return m__unnamed9; }
    void set__unnamed9(std::string _v) { m__dirty = true; m__unnamed9 = std::move(_v); }
    std::vector<std::unique_ptr<location_t>>* locations() const { return m_locations.get(); }
    void set_locations(std::unique_ptr<std::vector<std::unique_ptr<location_t>>> _v) { m__dirty = true; m_locations = std::move(_v); }
    misc_t* misc() const { return m_misc.get(); }
    void set_misc(std::unique_ptr<misc_t> _v) { m__dirty = true; m_misc = std::move(_v); }
    std::string _unnamed12() const { return m__unnamed12; }
    void set__unnamed12(std::string _v) { m__dirty = true; m__unnamed12 = std::move(_v); }
    step_edit_options_t* step_edit_options() const { return m_step_edit_options.get(); }
    void set_step_edit_options(std::unique_ptr<step_edit_options_t> _v) { m__dirty = true; m_step_edit_options = std::move(_v); }
    bool prog_change_to_seq() const { return m_prog_change_to_seq; }
    void set_prog_change_to_seq(bool _v) { m__dirty = true; m_prog_change_to_seq = std::move(_v); }
    std::string _unnamed15() const { return m__unnamed15; }
    void set__unnamed15(std::string _v) { m__dirty = true; m__unnamed15 = std::move(_v); }
    std::vector<std::unique_ptr<sequence_meta_t>>* sequences_metas() const { return m_sequences_metas.get(); }
    void set_sequences_metas(std::unique_ptr<std::vector<std::unique_ptr<sequence_meta_t>>> _v) { m__dirty = true; m_sequences_metas = std::move(_v); }
    std::vector<std::unique_ptr<song_t>>* songs() const { return m_songs.get(); }
    void set_songs(std::unique_ptr<std::vector<std::unique_ptr<song_t>>> _v) { m__dirty = true; m_songs = std::move(_v); }
    std::vector<std::unique_ptr<sequence_t>>* sequences() const { return m_sequences.get(); }
    void set_sequences(std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>> _v) { m__dirty = true; m_sequences = std::move(_v); }
    mpc2000xl_all_t* _root() const { return m__root; }
    void set__root(mpc2000xl_all_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::string m_magic;
    std::unique_ptr<defaults_t> m_defaults;
    std::unique_ptr<sequencer_t> m_sequencer;
    std::string m__unnamed3;
    std::unique_ptr<count_t> m_count;
    std::unique_ptr<midi_output_t> m_midi_output;
    std::unique_ptr<midi_input_t> m_midi_input;
    std::unique_ptr<midi_sync_t> m_midi_sync;
    std::unique_ptr<song_global_t> m_song_global;
    std::string m__unnamed9;
    std::unique_ptr<std::vector<std::unique_ptr<location_t>>> m_locations;
    std::unique_ptr<misc_t> m_misc;
    std::string m__unnamed12;
    std::unique_ptr<step_edit_options_t> m_step_edit_options;
    bool m_prog_change_to_seq;
    std::string m__unnamed15;
    std::unique_ptr<std::vector<std::unique_ptr<sequence_meta_t>>> m_sequences_metas;
    std::unique_ptr<std::vector<std::unique_ptr<song_t>>> m_songs;
    std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>> m_sequences;
    mpc2000xl_all_t* m__root;
    kaitai::kstruct* m__parent;
};

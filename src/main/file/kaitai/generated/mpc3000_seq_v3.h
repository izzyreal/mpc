#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc3000_seq_v3_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <set>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc3000_seq_v3_t : public kaitai::kstruct {

public:
    class bar_event_t;
    class ch_pressure_event_t;
    class control_change_event_t;
    class delays_t;
    class delta_time_event_t;
    class event_t;
    class mixer_t;
    class mixer_event_t;
    class note_event_t;
    class pitch_bend_event_t;
    class poly_pressure_event_t;
    class program_change_event_t;
    class sequence_header_t;
    class smpte_offset_t;
    class tempo_change_t;
    class track_header_t;
    class tune_request_event_t;
    class u3le_t;

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

    enum individual_out_t {
        INDIVIDUAL_OUT_UNASSIGNED = 0,
        INDIVIDUAL_OUT_OUT_1 = 1,
        INDIVIDUAL_OUT_OUT_2 = 2,
        INDIVIDUAL_OUT_OUT_3 = 3,
        INDIVIDUAL_OUT_OUT_4 = 4,
        INDIVIDUAL_OUT_OUT_5 = 5,
        INDIVIDUAL_OUT_OUT_6 = 6,
        INDIVIDUAL_OUT_OUT_7 = 7,
        INDIVIDUAL_OUT_OUT_8 = 8,
        INDIVIDUAL_OUT_INTERNAL_EFFECTS_GENERATOR = 9
    };
    static bool _is_defined_individual_out_t(individual_out_t v);

private:
    static const std::set<individual_out_t> _values_individual_out_t;

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

    enum no_yes_t {
        NO_YES_FALSE = 0,
        NO_YES_TRUE = 1
    };
    static bool _is_defined_no_yes_t(no_yes_t v);

private:
    static const std::set<no_yes_t> _values_no_yes_t;

public:

    enum off_on_t {
        OFF_ON_FALSE = 0,
        OFF_ON_TRUE = 1
    };
    static bool _is_defined_off_on_t(off_on_t v);

private:
    static const std::set<off_on_t> _values_off_on_t;

public:

    mpc3000_seq_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

private:
    void _read();

public:
    void _fetch_instances();
    ~mpc3000_seq_v3_t();

    class bar_event_t : public kaitai::kstruct {

    public:

        bar_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~bar_event_t();
        int32_t bar_number();
        uint8_t bar_number1() const { return m_bar_number1; }
        uint8_t bar_number2() const { return m_bar_number2; }
        uint8_t numerator() const { return m_numerator; }
        uint8_t denominator() const { return m_denominator; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_bar_number;
        int32_t m_bar_number;
        uint8_t m_bar_number1;
        uint8_t m_bar_number2;
        uint8_t m_numerator;
        uint8_t m_denominator;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class ch_pressure_event_t : public kaitai::kstruct {

    public:

        ch_pressure_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~ch_pressure_event_t();
        uint8_t pressure() const { return m_pressure; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_pressure;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class control_change_event_t : public kaitai::kstruct {

    public:

        control_change_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~control_change_event_t();
        controller_t controller() const { return m_controller; }
        uint8_t value() const { return m_value; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        controller_t m_controller;
        uint8_t m_value;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class delays_t : public kaitai::kstruct {

    public:

        delays_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~delays_t();
        uint8_t volume1() const { return m_volume1; }
        uint8_t volume2() const { return m_volume2; }
        uint8_t volume3() const { return m_volume3; }
        uint8_t pan1() const { return m_pan1; }
        uint8_t pan2() const { return m_pan2; }
        uint8_t pan3() const { return m_pan3; }
        uint16_t time1() const { return m_time1; }
        uint16_t time2() const { return m_time2; }
        uint16_t time3() const { return m_time3; }
        uint8_t feedback1() const { return m_feedback1; }
        uint8_t feedback2() const { return m_feedback2; }
        uint8_t feedback3() const { return m_feedback3; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_volume1;
        uint8_t m_volume2;
        uint8_t m_volume3;
        uint8_t m_pan1;
        uint8_t m_pan2;
        uint8_t m_pan3;
        uint16_t m_time1;
        uint16_t m_time2;
        uint16_t m_time3;
        uint8_t m_feedback1;
        uint8_t m_feedback2;
        uint8_t m_feedback3;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t* m__parent;
    };

    class delta_time_event_t : public kaitai::kstruct {

    public:

        delta_time_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~delta_time_event_t();
        uint16_t delta_time() const { return m_delta_time; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint16_t m_delta_time;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class event_t : public kaitai::kstruct {

    public:

        event_t(bool p_is_first_event, uint8_t p_preparsed_status, int32_t p_remaining_byte_count, kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~event_t();
        mpc3000_seq_v3_t::mixer_event_t* mixer_event();
        uint8_t next_status();
        uint8_t status();
        uint8_t parsed_status() const { return m_parsed_status; }
        uint8_t track_number() const { return m_track_number; }
        kaitai::kstruct* event_body() const { return m_event_body.get(); }
        std::vector<uint8_t>* system_exclusive_body() const { return m_system_exclusive_body.get(); }
        uint8_t parsed_next_status() const { return m_parsed_next_status; }
        bool is_first_event() const { return m_is_first_event; }
        uint8_t preparsed_status() const { return m_preparsed_status; }
        int32_t remaining_byte_count() const { return m_remaining_byte_count; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_mixer_event;
        std::unique_ptr<mpc3000_seq_v3_t::mixer_event_t> m_mixer_event;

    public:
        bool _is_null_mixer_event() { return !mixer_event(); };

    private:
        bool f_next_status;
        uint8_t m_next_status;
        bool f_status;
        uint8_t m_status;
        uint8_t m_parsed_status;
        bool n_parsed_status;

    public:
        bool _is_null_parsed_status() { parsed_status(); return n_parsed_status; };

    private:
        uint8_t m_track_number;
        bool n_track_number;

    public:
        bool _is_null_track_number() { track_number(); return n_track_number; };

    private:
        std::unique_ptr<kaitai::kstruct> m_event_body;

    public:
        bool _is_null_event_body() { return !event_body(); };

    private:
        std::unique_ptr<std::vector<uint8_t>> m_system_exclusive_body;

    public:
        bool _is_null_system_exclusive_body() { return !system_exclusive_body(); };

    private:
        uint8_t m_parsed_next_status;
        bool n_parsed_next_status;

    public:
        bool _is_null_parsed_next_status() { parsed_next_status(); return n_parsed_next_status; };

    private:
        bool m_is_first_event;
        uint8_t m_preparsed_status;
        int32_t m_remaining_byte_count;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t* m__parent;
    };

    class mixer_t : public kaitai::kstruct {

    public:

        mixer_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~mixer_t();
        uint8_t stereo_mix() const { return m_stereo_mix; }
        uint8_t stereo_pan() const { return m_stereo_pan; }
        uint8_t individual_out_mix() const { return m_individual_out_mix; }
        individual_out_t individual_out() const { return m_individual_out; }
        no_yes_t follow_stereo() const { return m_follow_stereo; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_stereo_mix;
        uint8_t m_stereo_pan;
        uint8_t m_individual_out_mix;
        individual_out_t m_individual_out;
        no_yes_t m_follow_stereo;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t* m__parent;
    };

    class mixer_event_t : public kaitai::kstruct {

    public:

        mixer_event_t(std::vector<uint8_t>* p_data, kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~mixer_event_t();
        uint8_t pad_index();
        mpc3000_seq_v3_t::mixer_event_param_t param();
        uint8_t value();
        std::vector<uint8_t>* data() const { return m_data; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_pad_index;
        uint8_t m_pad_index;
        bool f_param;
        mpc3000_seq_v3_t::mixer_event_param_t m_param;
        bool f_value;
        uint8_t m_value;
        std::vector<uint8_t>* m_data;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
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

        note_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~note_event_t();
        int32_t duration();
        mpc3000_seq_v3_t::note_event_t::note_variation_type_t note_variation_type();
        uint8_t note_number() const { return m_note_number; }
        uint8_t velocity() const { return m_velocity; }
        uint8_t note_variation_value() const { return m_note_variation_value; }
        uint8_t duration_byte_1() const { return m_duration_byte_1; }
        uint8_t duration_byte_2() const { return m_duration_byte_2; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_duration;
        int32_t m_duration;
        bool f_note_variation_type;
        mpc3000_seq_v3_t::note_event_t::note_variation_type_t m_note_variation_type;
        uint8_t m_note_number;
        uint8_t m_velocity;
        uint8_t m_note_variation_value;
        uint8_t m_duration_byte_1;
        uint8_t m_duration_byte_2;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class pitch_bend_event_t : public kaitai::kstruct {

    public:

        pitch_bend_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~pitch_bend_event_t();
        int32_t corrected_pitch_bend_amount();
        uint64_t pitch_bend_amount_bits_1() const { return m_pitch_bend_amount_bits_1; }
        uint64_t pitch_bend_amount_bits_2() const { return m_pitch_bend_amount_bits_2; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_corrected_pitch_bend_amount;
        int32_t m_corrected_pitch_bend_amount;
        uint64_t m_pitch_bend_amount_bits_1;
        uint64_t m_pitch_bend_amount_bits_2;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class poly_pressure_event_t : public kaitai::kstruct {

    public:

        poly_pressure_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~poly_pressure_event_t();
        uint8_t note() const { return m_note; }
        uint8_t pressure() const { return m_pressure; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_note;
        uint8_t m_pressure;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class program_change_event_t : public kaitai::kstruct {

    public:

        program_change_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~program_change_event_t();
        uint8_t program() const { return m_program; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_program;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class sequence_header_t : public kaitai::kstruct {

    public:

        sequence_header_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~sequence_header_t();
        uint8_t sequence_number() const { return m_sequence_number; }
        u3le_t* sequence_length_in_bytes() const { return m_sequence_length_in_bytes.get(); }
        std::string _unnamed2() const { return m__unnamed2; }
        std::string sequence_name() const { return m_sequence_name; }
        std::string _unnamed4() const { return m__unnamed4; }
        off_on_t loop_to_bar() const { return m_loop_to_bar; }
        uint64_t _unnamed6() const { return m__unnamed6; }
        uint16_t loop_to_bar_number() const { return m_loop_to_bar_number; }
        uint16_t number_of_bars() const { return m_number_of_bars; }
        uint32_t length_in_ticks() const { return m_length_in_ticks; }
        uint16_t tempo() const { return m_tempo; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_sequence_number;
        std::unique_ptr<u3le_t> m_sequence_length_in_bytes;
        std::string m__unnamed2;
        std::string m_sequence_name;
        std::string m__unnamed4;
        off_on_t m_loop_to_bar;
        uint64_t m__unnamed6;
        uint16_t m_loop_to_bar_number;
        uint16_t m_number_of_bars;
        uint32_t m_length_in_ticks;
        uint16_t m_tempo;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t* m__parent;
    };

    class smpte_offset_t : public kaitai::kstruct {

    public:

        smpte_offset_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~smpte_offset_t();
        uint8_t hundredth_frames() const { return m_hundredth_frames; }
        uint8_t frames() const { return m_frames; }
        uint8_t seconds() const { return m_seconds; }
        uint8_t minutes() const { return m_minutes; }
        uint8_t hours() const { return m_hours; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_hundredth_frames;
        uint8_t m_frames;
        uint8_t m_seconds;
        uint8_t m_minutes;
        uint8_t m_hours;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t* m__parent;
    };

    class tempo_change_t : public kaitai::kstruct {

    public:

        tempo_change_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~tempo_change_t();
        double factor_percentage();
        uint32_t ticks_from_sequence_start() const { return m_ticks_from_sequence_start; }
        uint64_t factor1() const { return m_factor1; }
        uint64_t factor2() const { return m_factor2; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_factor_percentage;
        double m_factor_percentage;
        uint32_t m_ticks_from_sequence_start;
        uint64_t m_factor1;
        uint64_t m_factor2;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t* m__parent;
    };

    class track_header_t : public kaitai::kstruct {

    public:

        track_header_t(kaitai::kstream* p__io, mpc3000_seq_v3_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~track_header_t();
        int8_t absolute_recorded_track_number() const { return m_absolute_recorded_track_number; }
        std::string _unnamed1() const { return m__unnamed1; }
        uint8_t user_track_number() const { return m_user_track_number; }
        bool track_mute() const { return m_track_mute; }
        bool track_in_use() const { return m_track_in_use; }
        bool drum_track() const { return m_drum_track; }
        uint64_t _unnamed6() const { return m__unnamed6; }
        uint8_t primary_port_channel_assignment() const { return m_primary_port_channel_assignment; }
        int8_t secondary_port_channel_assignment() const { return m_secondary_port_channel_assignment; }
        std::string track_name() const { return m_track_name; }
        uint8_t track_volume() const { return m_track_volume; }
        uint8_t program_change_number() const { return m_program_change_number; }
        std::string _unnamed12() const { return m__unnamed12; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        int8_t m_absolute_recorded_track_number;
        std::string m__unnamed1;
        bool n__unnamed1;

    public:
        bool _is_null__unnamed1() { _unnamed1(); return n__unnamed1; };

    private:
        uint8_t m_user_track_number;
        bool n_user_track_number;

    public:
        bool _is_null_user_track_number() { user_track_number(); return n_user_track_number; };

    private:
        bool m_track_mute;
        bool n_track_mute;

    public:
        bool _is_null_track_mute() { track_mute(); return n_track_mute; };

    private:
        bool m_track_in_use;
        bool n_track_in_use;

    public:
        bool _is_null_track_in_use() { track_in_use(); return n_track_in_use; };

    private:
        bool m_drum_track;
        bool n_drum_track;

    public:
        bool _is_null_drum_track() { drum_track(); return n_drum_track; };

    private:
        uint64_t m__unnamed6;
        bool n__unnamed6;

    public:
        bool _is_null__unnamed6() { _unnamed6(); return n__unnamed6; };

    private:
        uint8_t m_primary_port_channel_assignment;
        bool n_primary_port_channel_assignment;

    public:
        bool _is_null_primary_port_channel_assignment() { primary_port_channel_assignment(); return n_primary_port_channel_assignment; };

    private:
        int8_t m_secondary_port_channel_assignment;
        bool n_secondary_port_channel_assignment;

    public:
        bool _is_null_secondary_port_channel_assignment() { secondary_port_channel_assignment(); return n_secondary_port_channel_assignment; };

    private:
        std::string m_track_name;
        bool n_track_name;

    public:
        bool _is_null_track_name() { track_name(); return n_track_name; };

    private:
        uint8_t m_track_volume;
        bool n_track_volume;

    public:
        bool _is_null_track_volume() { track_volume(); return n_track_volume; };

    private:
        uint8_t m_program_change_number;
        bool n_program_change_number;

    public:
        bool _is_null_program_change_number() { program_change_number(); return n_program_change_number; };

    private:
        std::string m__unnamed12;
        bool n__unnamed12;

    public:
        bool _is_null__unnamed12() { _unnamed12(); return n__unnamed12; };

    private:
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t* m__parent;
    };

    class tune_request_event_t : public kaitai::kstruct {

    public:

        tune_request_event_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::event_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~tune_request_event_t();
        std::string tune_request_event() const { return m_tune_request_event; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        std::string m_tune_request_event;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::event_t* m__parent;
    };

    class u3le_t : public kaitai::kstruct {

    public:

        u3le_t(kaitai::kstream* p__io, mpc3000_seq_v3_t::sequence_header_t* p__parent = nullptr, mpc3000_seq_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~u3le_t();
        int32_t value();
        uint16_t b12() const { return m_b12; }
        uint8_t b3() const { return m_b3; }
        mpc3000_seq_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_value;
        int32_t m_value;
        uint16_t m_b12;
        uint8_t m_b3;
        mpc3000_seq_v3_t* m__root;
        mpc3000_seq_v3_t::sequence_header_t* m__parent;
    };

public:
    std::string file_id() const { return m_file_id; }
    std::string file_format_version() const { return m_file_format_version; }
    sequence_header_t* sequence_header() const { return m_sequence_header.get(); }
    smpte_offset_t* smpte_offset() const { return m_smpte_offset.get(); }
    std::vector<std::unique_ptr<mixer_t>>* mixer() const { return m_mixer.get(); }
    std::string _unnamed5() const { return m__unnamed5; }
    delays_t* delays() const { return m_delays.get(); }
    std::string _unnamed7() const { return m__unnamed7; }
    std::string _unnamed8() const { return m__unnamed8; }
    uint8_t last_active_track() const { return m_last_active_track; }
    uint8_t number_of_tempo_changes() const { return m_number_of_tempo_changes; }
    uint8_t number_of_active_track_headers() const { return m_number_of_active_track_headers; }
    std::vector<std::unique_ptr<track_header_t>>* track_headers() const { return m_track_headers.get(); }
    std::vector<std::unique_ptr<tempo_change_t>>* tempo_changes() const { return m_tempo_changes.get(); }
    std::vector<std::unique_ptr<event_t>>* events() const { return m_events.get(); }
    mpc3000_seq_v3_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }

private:
    std::string m_file_id;
    std::string m_file_format_version;
    std::unique_ptr<sequence_header_t> m_sequence_header;
    std::unique_ptr<smpte_offset_t> m_smpte_offset;
    std::unique_ptr<std::vector<std::unique_ptr<mixer_t>>> m_mixer;
    std::string m__unnamed5;
    std::unique_ptr<delays_t> m_delays;
    std::string m__unnamed7;
    std::string m__unnamed8;
    uint8_t m_last_active_track;
    uint8_t m_number_of_tempo_changes;
    uint8_t m_number_of_active_track_headers;
    std::unique_ptr<std::vector<std::unique_ptr<track_header_t>>> m_track_headers;
    std::unique_ptr<std::vector<std::unique_ptr<tempo_change_t>>> m_tempo_changes;
    std::unique_ptr<std::vector<std::unique_ptr<event_t>>> m_events;
    mpc3000_seq_v3_t* m__root;
    kaitai::kstruct* m__parent;
};

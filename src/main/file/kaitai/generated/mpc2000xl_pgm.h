#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc2000xl_pgm_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <set>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc2000xl_pgm_t : public kaitai::kstruct {

public:
    class note_t;
    class pad_mixer_t;
    class slider_t;

    enum decay_mode_t {
        DECAY_MODE_END = 0,
        DECAY_MODE_START = 1
    };
    static bool _is_defined_decay_mode_t(decay_mode_t v);

private:
    static const std::set<decay_mode_t> _values_decay_mode_t;

public:

    enum fx_output_t {
        FX_OUTPUT_NONE = 0,
        FX_OUTPUT_M1 = 1,
        FX_OUTPUT_M2 = 2,
        FX_OUTPUT_R1 = 3,
        FX_OUTPUT_R2 = 4
    };
    static bool _is_defined_fx_output_t(fx_output_t v);

private:
    static const std::set<fx_output_t> _values_fx_output_t;

public:

    enum slider_parameter_t {
        SLIDER_PARAMETER_TUNING = 0,
        SLIDER_PARAMETER_DECAY = 1,
        SLIDER_PARAMETER_ATTACK = 2,
        SLIDER_PARAMETER_FILTER = 3
    };
    static bool _is_defined_slider_parameter_t(slider_parameter_t v);

private:
    static const std::set<slider_parameter_t> _values_slider_parameter_t;

public:

    enum sound_generation_mode_t {
        SOUND_GENERATION_MODE_NORMAL = 0,
        SOUND_GENERATION_MODE_SIMULT = 1,
        SOUND_GENERATION_MODE_VEL_SW = 2
    };
    static bool _is_defined_sound_generation_mode_t(sound_generation_mode_t v);

private:
    static const std::set<sound_generation_mode_t> _values_sound_generation_mode_t;

public:

    enum voice_overlap_mode_t {
        VOICE_OVERLAP_MODE_POLY = 0,
        VOICE_OVERLAP_MODE_MONO = 1,
        VOICE_OVERLAP_MODE_NOTE_OFF = 2
    };
    static bool _is_defined_voice_overlap_mode_t(voice_overlap_mode_t v);

private:
    static const std::set<voice_overlap_mode_t> _values_voice_overlap_mode_t;

public:

private:
    bool m__dirty;

public:

    mpc2000xl_pgm_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc2000xl_pgm_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc2000xl_pgm_t();

    class note_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        note_t(kaitai::kstream* p__io, mpc2000xl_pgm_t* p__parent = nullptr, mpc2000xl_pgm_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~note_t();
        uint8_t sound_index() const { return m_sound_index; }
        void set_sound_index(uint8_t _v) { m__dirty = true; m_sound_index = std::move(_v); }
        sound_generation_mode_t sound_generation_mode() const { return m_sound_generation_mode; }
        void set_sound_generation_mode(sound_generation_mode_t _v) { m__dirty = true; m_sound_generation_mode = std::move(_v); }
        uint8_t velocity_range_lower() const { return m_velocity_range_lower; }
        void set_velocity_range_lower(uint8_t _v) { m__dirty = true; m_velocity_range_lower = std::move(_v); }
        uint8_t also_play_use_note_1() const { return m_also_play_use_note_1; }
        void set_also_play_use_note_1(uint8_t _v) { m__dirty = true; m_also_play_use_note_1 = std::move(_v); }
        uint8_t velocity_range_upper() const { return m_velocity_range_upper; }
        void set_velocity_range_upper(uint8_t _v) { m__dirty = true; m_velocity_range_upper = std::move(_v); }
        uint8_t also_play_use_note_2() const { return m_also_play_use_note_2; }
        void set_also_play_use_note_2(uint8_t _v) { m__dirty = true; m_also_play_use_note_2 = std::move(_v); }
        voice_overlap_mode_t voice_overlap_mode() const { return m_voice_overlap_mode; }
        void set_voice_overlap_mode(voice_overlap_mode_t _v) { m__dirty = true; m_voice_overlap_mode = std::move(_v); }
        uint8_t mute_assign_1() const { return m_mute_assign_1; }
        void set_mute_assign_1(uint8_t _v) { m__dirty = true; m_mute_assign_1 = std::move(_v); }
        uint8_t mute_assign_2() const { return m_mute_assign_2; }
        void set_mute_assign_2(uint8_t _v) { m__dirty = true; m_mute_assign_2 = std::move(_v); }
        int16_t tune() const { return m_tune; }
        void set_tune(int16_t _v) { m__dirty = true; m_tune = std::move(_v); }
        uint8_t attack() const { return m_attack; }
        void set_attack(uint8_t _v) { m__dirty = true; m_attack = std::move(_v); }
        uint8_t decay() const { return m_decay; }
        void set_decay(uint8_t _v) { m__dirty = true; m_decay = std::move(_v); }
        decay_mode_t decay_mode() const { return m_decay_mode; }
        void set_decay_mode(decay_mode_t _v) { m__dirty = true; m_decay_mode = std::move(_v); }
        uint8_t cutoff() const { return m_cutoff; }
        void set_cutoff(uint8_t _v) { m__dirty = true; m_cutoff = std::move(_v); }
        uint8_t resonance() const { return m_resonance; }
        void set_resonance(uint8_t _v) { m__dirty = true; m_resonance = std::move(_v); }
        uint8_t velocity_envelope_to_filter_attack() const { return m_velocity_envelope_to_filter_attack; }
        void set_velocity_envelope_to_filter_attack(uint8_t _v) { m__dirty = true; m_velocity_envelope_to_filter_attack = std::move(_v); }
        uint8_t velocity_envelope_to_filter_decay() const { return m_velocity_envelope_to_filter_decay; }
        void set_velocity_envelope_to_filter_decay(uint8_t _v) { m__dirty = true; m_velocity_envelope_to_filter_decay = std::move(_v); }
        uint8_t velocity_envelope_to_filter_amount() const { return m_velocity_envelope_to_filter_amount; }
        void set_velocity_envelope_to_filter_amount(uint8_t _v) { m__dirty = true; m_velocity_envelope_to_filter_amount = std::move(_v); }
        uint8_t velocity_to_level() const { return m_velocity_to_level; }
        void set_velocity_to_level(uint8_t _v) { m__dirty = true; m_velocity_to_level = std::move(_v); }
        uint8_t velocity_to_attack() const { return m_velocity_to_attack; }
        void set_velocity_to_attack(uint8_t _v) { m__dirty = true; m_velocity_to_attack = std::move(_v); }
        uint8_t velocity_to_start() const { return m_velocity_to_start; }
        void set_velocity_to_start(uint8_t _v) { m__dirty = true; m_velocity_to_start = std::move(_v); }
        uint8_t velocity_to_cutoff() const { return m_velocity_to_cutoff; }
        void set_velocity_to_cutoff(uint8_t _v) { m__dirty = true; m_velocity_to_cutoff = std::move(_v); }
        slider_parameter_t slider_parameter() const { return m_slider_parameter; }
        void set_slider_parameter(slider_parameter_t _v) { m__dirty = true; m_slider_parameter = std::move(_v); }
        uint8_t velocity_to_pitch() const { return m_velocity_to_pitch; }
        void set_velocity_to_pitch(uint8_t _v) { m__dirty = true; m_velocity_to_pitch = std::move(_v); }
        mpc2000xl_pgm_t* _root() const { return m__root; }
        void set__root(mpc2000xl_pgm_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_pgm_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_sound_index;
        sound_generation_mode_t m_sound_generation_mode;
        uint8_t m_velocity_range_lower;
        uint8_t m_also_play_use_note_1;
        uint8_t m_velocity_range_upper;
        uint8_t m_also_play_use_note_2;
        voice_overlap_mode_t m_voice_overlap_mode;
        uint8_t m_mute_assign_1;
        uint8_t m_mute_assign_2;
        int16_t m_tune;
        uint8_t m_attack;
        uint8_t m_decay;
        decay_mode_t m_decay_mode;
        uint8_t m_cutoff;
        uint8_t m_resonance;
        uint8_t m_velocity_envelope_to_filter_attack;
        uint8_t m_velocity_envelope_to_filter_decay;
        uint8_t m_velocity_envelope_to_filter_amount;
        uint8_t m_velocity_to_level;
        uint8_t m_velocity_to_attack;
        uint8_t m_velocity_to_start;
        uint8_t m_velocity_to_cutoff;
        slider_parameter_t m_slider_parameter;
        uint8_t m_velocity_to_pitch;
        mpc2000xl_pgm_t* m__root;
        mpc2000xl_pgm_t* m__parent;
    };

    class pad_mixer_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        pad_mixer_t(kaitai::kstream* p__io, mpc2000xl_pgm_t* p__parent = nullptr, mpc2000xl_pgm_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~pad_mixer_t();
        fx_output_t fx_output() const { return m_fx_output; }
        void set_fx_output(fx_output_t _v) { m__dirty = true; m_fx_output = std::move(_v); }
        uint8_t volume() const { return m_volume; }
        void set_volume(uint8_t _v) { m__dirty = true; m_volume = std::move(_v); }
        uint8_t pan() const { return m_pan; }
        void set_pan(uint8_t _v) { m__dirty = true; m_pan = std::move(_v); }
        uint8_t volume_individual() const { return m_volume_individual; }
        void set_volume_individual(uint8_t _v) { m__dirty = true; m_volume_individual = std::move(_v); }
        uint8_t output() const { return m_output; }
        void set_output(uint8_t _v) { m__dirty = true; m_output = std::move(_v); }
        uint8_t effects_send_level() const { return m_effects_send_level; }
        void set_effects_send_level(uint8_t _v) { m__dirty = true; m_effects_send_level = std::move(_v); }
        mpc2000xl_pgm_t* _root() const { return m__root; }
        void set__root(mpc2000xl_pgm_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_pgm_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        fx_output_t m_fx_output;
        uint8_t m_volume;
        uint8_t m_pan;
        uint8_t m_volume_individual;
        uint8_t m_output;
        uint8_t m_effects_send_level;
        mpc2000xl_pgm_t* m__root;
        mpc2000xl_pgm_t* m__parent;
    };

    class slider_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        slider_t(kaitai::kstream* p__io, mpc2000xl_pgm_t* p__parent = nullptr, mpc2000xl_pgm_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~slider_t();
        uint8_t note() const { return m_note; }
        void set_note(uint8_t _v) { m__dirty = true; m_note = std::move(_v); }
        int8_t tune_low() const { return m_tune_low; }
        void set_tune_low(int8_t _v) { m__dirty = true; m_tune_low = std::move(_v); }
        int8_t tune_high() const { return m_tune_high; }
        void set_tune_high(int8_t _v) { m__dirty = true; m_tune_high = std::move(_v); }
        int8_t decay_low() const { return m_decay_low; }
        void set_decay_low(int8_t _v) { m__dirty = true; m_decay_low = std::move(_v); }
        int8_t decay_high() const { return m_decay_high; }
        void set_decay_high(int8_t _v) { m__dirty = true; m_decay_high = std::move(_v); }
        int8_t attack_low() const { return m_attack_low; }
        void set_attack_low(int8_t _v) { m__dirty = true; m_attack_low = std::move(_v); }
        int8_t attack_high() const { return m_attack_high; }
        void set_attack_high(int8_t _v) { m__dirty = true; m_attack_high = std::move(_v); }
        int8_t filter_low() const { return m_filter_low; }
        void set_filter_low(int8_t _v) { m__dirty = true; m_filter_low = std::move(_v); }
        int8_t filter_high() const { return m_filter_high; }
        void set_filter_high(int8_t _v) { m__dirty = true; m_filter_high = std::move(_v); }
        mpc2000xl_pgm_t* _root() const { return m__root; }
        void set__root(mpc2000xl_pgm_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_pgm_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_note;
        int8_t m_tune_low;
        int8_t m_tune_high;
        int8_t m_decay_low;
        int8_t m_decay_high;
        int8_t m_attack_low;
        int8_t m_attack_high;
        int8_t m_filter_low;
        int8_t m_filter_high;
        mpc2000xl_pgm_t* m__root;
        mpc2000xl_pgm_t* m__parent;
    };

public:
    std::string magic() const { return m_magic; }
    void set_magic(std::string _v) { m__dirty = true; m_magic = std::move(_v); }
    uint16_t sound_count() const { return m_sound_count; }
    void set_sound_count(uint16_t _v) { m__dirty = true; m_sound_count = std::move(_v); }
    std::vector<std::string>* sound_names() const { return m_sound_names.get(); }
    void set_sound_names(std::unique_ptr<std::vector<std::string>> _v) { m__dirty = true; m_sound_names = std::move(_v); }
    std::string _unnamed3() const { return m__unnamed3; }
    void set__unnamed3(std::string _v) { m__dirty = true; m__unnamed3 = std::move(_v); }
    std::string name() const { return m_name; }
    void set_name(std::string _v) { m__dirty = true; m_name = std::move(_v); }
    slider_t* slider() const { return m_slider.get(); }
    void set_slider(std::unique_ptr<slider_t> _v) { m__dirty = true; m_slider = std::move(_v); }
    uint8_t program_change() const { return m_program_change; }
    void set_program_change(uint8_t _v) { m__dirty = true; m_program_change = std::move(_v); }
    std::string _unnamed7() const { return m__unnamed7; }
    void set__unnamed7(std::string _v) { m__dirty = true; m__unnamed7 = std::move(_v); }
    std::vector<std::unique_ptr<note_t>>* note_parameters() const { return m_note_parameters.get(); }
    void set_note_parameters(std::unique_ptr<std::vector<std::unique_ptr<note_t>>> _v) { m__dirty = true; m_note_parameters = std::move(_v); }
    std::string _unnamed9() const { return m__unnamed9; }
    void set__unnamed9(std::string _v) { m__dirty = true; m__unnamed9 = std::move(_v); }
    std::vector<std::unique_ptr<pad_mixer_t>>* pad_mixers() const { return m_pad_mixers.get(); }
    void set_pad_mixers(std::unique_ptr<std::vector<std::unique_ptr<pad_mixer_t>>> _v) { m__dirty = true; m_pad_mixers = std::move(_v); }
    std::string _unnamed11() const { return m__unnamed11; }
    void set__unnamed11(std::string _v) { m__dirty = true; m__unnamed11 = std::move(_v); }
    std::vector<int8_t>* pad_to_note_mapping() const { return m_pad_to_note_mapping.get(); }
    void set_pad_to_note_mapping(std::unique_ptr<std::vector<int8_t>> _v) { m__dirty = true; m_pad_to_note_mapping = std::move(_v); }
    std::string _unnamed13() const { return m__unnamed13; }
    void set__unnamed13(std::string _v) { m__dirty = true; m__unnamed13 = std::move(_v); }
    mpc2000xl_pgm_t* _root() const { return m__root; }
    void set__root(mpc2000xl_pgm_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::string m_magic;
    uint16_t m_sound_count;
    std::unique_ptr<std::vector<std::string>> m_sound_names;
    std::string m__unnamed3;
    std::string m_name;
    std::unique_ptr<slider_t> m_slider;
    uint8_t m_program_change;
    std::string m__unnamed7;
    std::unique_ptr<std::vector<std::unique_ptr<note_t>>> m_note_parameters;
    std::string m__unnamed9;
    std::unique_ptr<std::vector<std::unique_ptr<pad_mixer_t>>> m_pad_mixers;
    std::string m__unnamed11;
    std::unique_ptr<std::vector<int8_t>> m_pad_to_note_mapping;
    std::string m__unnamed13;
    mpc2000xl_pgm_t* m__root;
    kaitai::kstruct* m__parent;
};

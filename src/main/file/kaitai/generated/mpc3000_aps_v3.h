#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc3000_aps_v3_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <set>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc3000_aps_v3_t : public kaitai::kstruct {

public:
    class effects_settings_t;
    class mixer_settings_t;
    class note_variation_t;
    class program_t;
    class sound_assignment_t;

    enum mix_source_t {
        MIX_SOURCE_MASTER = 0,
        MIX_SOURCE_SEQUENCE = 1,
        MIX_SOURCE_PROGRAM = 2
    };
    static bool _is_defined_mix_source_t(mix_source_t v);

private:
    static const std::set<mix_source_t> _values_mix_source_t;

public:

private:
    bool m__dirty;

public:

    mpc3000_aps_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc3000_aps_v3_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc3000_aps_v3_t();

    class effects_settings_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        effects_settings_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc3000_aps_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~effects_settings_t();
        bool effects_on() const { return m_effects_on; }
        void set_effects_on(bool _v) { m__dirty = true; m_effects_on = std::move(_v); }
        uint64_t _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(uint64_t _v) { m__dirty = true; m__unnamed1 = std::move(_v); }

        /**
         * Unused
         */
        uint8_t _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(uint8_t _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        uint8_t delay_volume_tap1() const { return m_delay_volume_tap1; }
        void set_delay_volume_tap1(uint8_t _v) { m__dirty = true; m_delay_volume_tap1 = std::move(_v); }
        uint8_t delay_volume_tap2() const { return m_delay_volume_tap2; }
        void set_delay_volume_tap2(uint8_t _v) { m__dirty = true; m_delay_volume_tap2 = std::move(_v); }
        uint8_t delay_volume_tap3() const { return m_delay_volume_tap3; }
        void set_delay_volume_tap3(uint8_t _v) { m__dirty = true; m_delay_volume_tap3 = std::move(_v); }
        uint8_t delay_pan_tap1() const { return m_delay_pan_tap1; }
        void set_delay_pan_tap1(uint8_t _v) { m__dirty = true; m_delay_pan_tap1 = std::move(_v); }
        uint8_t delay_pan_tap2() const { return m_delay_pan_tap2; }
        void set_delay_pan_tap2(uint8_t _v) { m__dirty = true; m_delay_pan_tap2 = std::move(_v); }
        uint8_t delay_pan_tap3() const { return m_delay_pan_tap3; }
        void set_delay_pan_tap3(uint8_t _v) { m__dirty = true; m_delay_pan_tap3 = std::move(_v); }
        uint16_t delay_msecs_tap1() const { return m_delay_msecs_tap1; }
        void set_delay_msecs_tap1(uint16_t _v) { m__dirty = true; m_delay_msecs_tap1 = std::move(_v); }
        uint16_t delay_msecs_tap2() const { return m_delay_msecs_tap2; }
        void set_delay_msecs_tap2(uint16_t _v) { m__dirty = true; m_delay_msecs_tap2 = std::move(_v); }
        uint16_t delay_msecs_tap3() const { return m_delay_msecs_tap3; }
        void set_delay_msecs_tap3(uint16_t _v) { m__dirty = true; m_delay_msecs_tap3 = std::move(_v); }

        /**
         * Unused
         */
        std::string _unnamed12() const { return m__unnamed12; }
        void set__unnamed12(std::string _v) { m__dirty = true; m__unnamed12 = std::move(_v); }
        uint8_t delay_feedback_tap1() const { return m_delay_feedback_tap1; }
        void set_delay_feedback_tap1(uint8_t _v) { m__dirty = true; m_delay_feedback_tap1 = std::move(_v); }
        uint8_t delay_feedback_tap2() const { return m_delay_feedback_tap2; }
        void set_delay_feedback_tap2(uint8_t _v) { m__dirty = true; m_delay_feedback_tap2 = std::move(_v); }
        uint8_t delay_feedback_tap3() const { return m_delay_feedback_tap3; }
        void set_delay_feedback_tap3(uint8_t _v) { m__dirty = true; m_delay_feedback_tap3 = std::move(_v); }

        /**
         * Unused
         */
        std::string _unnamed16() const { return m__unnamed16; }
        void set__unnamed16(std::string _v) { m__dirty = true; m__unnamed16 = std::move(_v); }
        mpc3000_aps_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_aps_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        bool m_effects_on;
        uint64_t m__unnamed1;
        uint8_t m__unnamed2;
        uint8_t m_delay_volume_tap1;
        uint8_t m_delay_volume_tap2;
        uint8_t m_delay_volume_tap3;
        uint8_t m_delay_pan_tap1;
        uint8_t m_delay_pan_tap2;
        uint8_t m_delay_pan_tap3;
        uint16_t m_delay_msecs_tap1;
        uint16_t m_delay_msecs_tap2;
        uint16_t m_delay_msecs_tap3;
        std::string m__unnamed12;
        uint8_t m_delay_feedback_tap1;
        uint8_t m_delay_feedback_tap2;
        uint8_t m_delay_feedback_tap3;
        std::string m__unnamed16;
        mpc3000_aps_v3_t* m__root;
        kaitai::kstruct* m__parent;
    };

    class mixer_settings_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        mixer_settings_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc3000_aps_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~mixer_settings_t();
        uint8_t stereo_mix_volume() const { return m_stereo_mix_volume; }
        void set_stereo_mix_volume(uint8_t _v) { m__dirty = true; m_stereo_mix_volume = std::move(_v); }
        uint8_t stereo_mix_pan() const { return m_stereo_mix_pan; }
        void set_stereo_mix_pan(uint8_t _v) { m__dirty = true; m_stereo_mix_pan = std::move(_v); }
        uint8_t echo_volume() const { return m_echo_volume; }
        void set_echo_volume(uint8_t _v) { m__dirty = true; m_echo_volume = std::move(_v); }
        uint8_t out_assign_fol_ster() const { return m_out_assign_fol_ster; }
        void set_out_assign_fol_ster(uint8_t _v) { m__dirty = true; m_out_assign_fol_ster = std::move(_v); }
        mpc3000_aps_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_aps_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_stereo_mix_volume;
        uint8_t m_stereo_mix_pan;
        uint8_t m_echo_volume;
        uint8_t m_out_assign_fol_ster;
        mpc3000_aps_v3_t* m__root;
        kaitai::kstruct* m__parent;
    };

    class note_variation_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        note_variation_t(kaitai::kstream* p__io, mpc3000_aps_v3_t::program_t* p__parent = nullptr, mpc3000_aps_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~note_variation_t();
        uint8_t note_number_assignment() const { return m_note_number_assignment; }
        void set_note_number_assignment(uint8_t _v) { m__dirty = true; m_note_number_assignment = std::move(_v); }
        uint8_t tuning_low_range() const { return m_tuning_low_range; }
        void set_tuning_low_range(uint8_t _v) { m__dirty = true; m_tuning_low_range = std::move(_v); }
        uint8_t tuning_hi_range() const { return m_tuning_hi_range; }
        void set_tuning_hi_range(uint8_t _v) { m__dirty = true; m_tuning_hi_range = std::move(_v); }
        uint8_t attack_low_range() const { return m_attack_low_range; }
        void set_attack_low_range(uint8_t _v) { m__dirty = true; m_attack_low_range = std::move(_v); }
        uint8_t attack_hi_range() const { return m_attack_hi_range; }
        void set_attack_hi_range(uint8_t _v) { m__dirty = true; m_attack_hi_range = std::move(_v); }
        uint8_t decay_low_range() const { return m_decay_low_range; }
        void set_decay_low_range(uint8_t _v) { m__dirty = true; m_decay_low_range = std::move(_v); }
        uint8_t decay_hi_range() const { return m_decay_hi_range; }
        void set_decay_hi_range(uint8_t _v) { m__dirty = true; m_decay_hi_range = std::move(_v); }
        uint8_t filter_low_range() const { return m_filter_low_range; }
        void set_filter_low_range(uint8_t _v) { m__dirty = true; m_filter_low_range = std::move(_v); }
        uint8_t filter_hi_range() const { return m_filter_hi_range; }
        void set_filter_hi_range(uint8_t _v) { m__dirty = true; m_filter_hi_range = std::move(_v); }
        mpc3000_aps_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_aps_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc3000_aps_v3_t::program_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_note_number_assignment;
        uint8_t m_tuning_low_range;
        uint8_t m_tuning_hi_range;
        uint8_t m_attack_low_range;
        uint8_t m_attack_hi_range;
        uint8_t m_decay_low_range;
        uint8_t m_decay_hi_range;
        uint8_t m_filter_low_range;
        uint8_t m_filter_hi_range;
        mpc3000_aps_v3_t* m__root;
        mpc3000_aps_v3_t::program_t* m__parent;
    };

    class program_t : public kaitai::kstruct {

    public:
        class pad_note_number_assignment_t;

    private:
        bool m__dirty;

    public:

        program_t(kaitai::kstream* p__io, mpc3000_aps_v3_t* p__parent = nullptr, mpc3000_aps_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~program_t();

        class pad_note_number_assignment_t : public kaitai::kstruct {

        public:

        private:
            bool m__dirty;

        public:

            pad_note_number_assignment_t(kaitai::kstream* p__io, mpc3000_aps_v3_t::program_t* p__parent = nullptr, mpc3000_aps_v3_t* p__root = nullptr);
            void _read();
            void _fetch_instances();
            void _write();
            void _check();
            ~pad_note_number_assignment_t();
            uint8_t note_number() const { return m_note_number; }
            void set_note_number(uint8_t _v) { m__dirty = true; m_note_number = std::move(_v); }
            mpc3000_aps_v3_t* _root() const { return m__root; }
            void set__root(mpc3000_aps_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
            kaitai::kstruct* _parent() const { return m__parent; }
            void set__parent(mpc3000_aps_v3_t::program_t* _v) { m__dirty = true; m__parent = std::move(_v); }

        private:
            uint8_t m_note_number;
            mpc3000_aps_v3_t* m__root;
            mpc3000_aps_v3_t::program_t* m__parent;
        };

    public:
        std::string program_name() const { return m_program_name; }
        void set_program_name(std::string _v) { m__dirty = true; m_program_name = std::move(_v); }
        note_variation_t* note_variation() const { return m_note_variation.get(); }
        void set_note_variation(std::unique_ptr<note_variation_t> _v) { m__dirty = true; m_note_variation = std::move(_v); }
        effects_settings_t* effects_settings() const { return m_effects_settings.get(); }
        void set_effects_settings(std::unique_ptr<effects_settings_t> _v) { m__dirty = true; m_effects_settings = std::move(_v); }
        std::vector<std::unique_ptr<sound_assignment_t>>* sound_assignments() const { return m_sound_assignments.get(); }
        void set_sound_assignments(std::unique_ptr<std::vector<std::unique_ptr<sound_assignment_t>>> _v) { m__dirty = true; m_sound_assignments = std::move(_v); }
        std::vector<std::unique_ptr<mixer_settings_t>>* mixer_settings() const { return m_mixer_settings.get(); }
        void set_mixer_settings(std::unique_ptr<std::vector<std::unique_ptr<mixer_settings_t>>> _v) { m__dirty = true; m_mixer_settings = std::move(_v); }

        /**
         * Note number assignments for pads A01..D16
         */
        std::vector<std::unique_ptr<pad_note_number_assignment_t>>* pad_note_number_assignments() const { return m_pad_note_number_assignments.get(); }
        void set_pad_note_number_assignments(std::unique_ptr<std::vector<std::unique_ptr<pad_note_number_assignment_t>>> _v) { m__dirty = true; m_pad_note_number_assignments = std::move(_v); }
        mpc3000_aps_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_aps_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc3000_aps_v3_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m_program_name;
        std::unique_ptr<note_variation_t> m_note_variation;
        std::unique_ptr<effects_settings_t> m_effects_settings;
        std::unique_ptr<std::vector<std::unique_ptr<sound_assignment_t>>> m_sound_assignments;
        std::unique_ptr<std::vector<std::unique_ptr<mixer_settings_t>>> m_mixer_settings;
        std::unique_ptr<std::vector<std::unique_ptr<pad_note_number_assignment_t>>> m_pad_note_number_assignments;
        mpc3000_aps_v3_t* m__root;
        mpc3000_aps_v3_t* m__parent;
    };

    class sound_assignment_t : public kaitai::kstruct {

    public:

        enum decay_mode_t {
            DECAY_MODE_END = 0,
            DECAY_MODE_START = 1
        };
        static bool _is_defined_decay_mode_t(decay_mode_t v);

    private:
        static const std::set<decay_mode_t> _values_decay_mode_t;

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

        enum poly_mode_t {
            POLY_MODE_POLY = 0,
            POLY_MODE_MONO = 1,
            POLY_MODE_NOTE_OFF = 2
        };
        static bool _is_defined_poly_mode_t(poly_mode_t v);

    private:
        static const std::set<poly_mode_t> _values_poly_mode_t;

    public:

        enum sound_generator_mode_t {
            SOUND_GENERATOR_MODE_NORMAL = 0,
            SOUND_GENERATOR_MODE_SIMULTANEOUS = 1,
            SOUND_GENERATOR_MODE_VELOCITY_SWITCH = 2,
            SOUND_GENERATOR_MODE_DECAY_SWITCH = 3
        };
        static bool _is_defined_sound_generator_mode_t(sound_generator_mode_t v);

    private:
        static const std::set<sound_generator_mode_t> _values_sound_generator_mode_t;

    public:

    private:
        bool m__dirty;

    public:

        sound_assignment_t(kaitai::kstream* p__io, mpc3000_aps_v3_t::program_t* p__parent = nullptr, mpc3000_aps_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sound_assignment_t();
        uint8_t sound_number() const { return m_sound_number; }
        void set_sound_number(uint8_t _v) { m__dirty = true; m_sound_number = std::move(_v); }
        sound_generator_mode_t sound_generator_mode() const { return m_sound_generator_mode; }
        void set_sound_generator_mode(sound_generator_mode_t _v) { m__dirty = true; m_sound_generator_mode = std::move(_v); }
        uint8_t if_over1() const { return m_if_over1; }
        void set_if_over1(uint8_t _v) { m__dirty = true; m_if_over1 = std::move(_v); }
        uint8_t use_also_plays1() const { return m_use_also_plays1; }
        void set_use_also_plays1(uint8_t _v) { m__dirty = true; m_use_also_plays1 = std::move(_v); }
        uint8_t if_over2() const { return m_if_over2; }
        void set_if_over2(uint8_t _v) { m__dirty = true; m_if_over2 = std::move(_v); }
        uint8_t use_also_plays2() const { return m_use_also_plays2; }
        void set_use_also_plays2(uint8_t _v) { m__dirty = true; m_use_also_plays2 = std::move(_v); }
        poly_mode_t poly() const { return m_poly; }
        void set_poly(poly_mode_t _v) { m__dirty = true; m_poly = std::move(_v); }
        uint8_t cutoff_note_1() const { return m_cutoff_note_1; }
        void set_cutoff_note_1(uint8_t _v) { m__dirty = true; m_cutoff_note_1 = std::move(_v); }
        uint8_t cutoff_note_2() const { return m_cutoff_note_2; }
        void set_cutoff_note_2(uint8_t _v) { m__dirty = true; m_cutoff_note_2 = std::move(_v); }
        int16_t tune() const { return m_tune; }
        void set_tune(int16_t _v) { m__dirty = true; m_tune = std::move(_v); }
        uint8_t attack() const { return m_attack; }
        void set_attack(uint8_t _v) { m__dirty = true; m_attack = std::move(_v); }
        uint8_t decay() const { return m_decay; }
        void set_decay(uint8_t _v) { m__dirty = true; m_decay = std::move(_v); }
        decay_mode_t decay_mode() const { return m_decay_mode; }
        void set_decay_mode(decay_mode_t _v) { m__dirty = true; m_decay_mode = std::move(_v); }
        uint8_t filter_frequency() const { return m_filter_frequency; }
        void set_filter_frequency(uint8_t _v) { m__dirty = true; m_filter_frequency = std::move(_v); }
        uint8_t filter_resonance() const { return m_filter_resonance; }
        void set_filter_resonance(uint8_t _v) { m__dirty = true; m_filter_resonance = std::move(_v); }
        uint8_t filter_envel_attack() const { return m_filter_envel_attack; }
        void set_filter_envel_attack(uint8_t _v) { m__dirty = true; m_filter_envel_attack = std::move(_v); }
        uint8_t filter_envel_decay() const { return m_filter_envel_decay; }
        void set_filter_envel_decay(uint8_t _v) { m__dirty = true; m_filter_envel_decay = std::move(_v); }
        uint8_t filter_envel_amount() const { return m_filter_envel_amount; }
        void set_filter_envel_amount(uint8_t _v) { m__dirty = true; m_filter_envel_amount = std::move(_v); }
        uint8_t veloc_mod_of_volume() const { return m_veloc_mod_of_volume; }
        void set_veloc_mod_of_volume(uint8_t _v) { m__dirty = true; m_veloc_mod_of_volume = std::move(_v); }
        uint8_t veloc_mod_of_attack() const { return m_veloc_mod_of_attack; }
        void set_veloc_mod_of_attack(uint8_t _v) { m__dirty = true; m_veloc_mod_of_attack = std::move(_v); }
        uint8_t veloc_mod_of_soft_start() const { return m_veloc_mod_of_soft_start; }
        void set_veloc_mod_of_soft_start(uint8_t _v) { m__dirty = true; m_veloc_mod_of_soft_start = std::move(_v); }
        uint8_t veloc_mod_of_filter_freq() const { return m_veloc_mod_of_filter_freq; }
        void set_veloc_mod_of_filter_freq(uint8_t _v) { m__dirty = true; m_veloc_mod_of_filter_freq = std::move(_v); }
        note_variation_type_t param() const { return m_param; }
        void set_param(note_variation_type_t _v) { m__dirty = true; m_param = std::move(_v); }
        mpc3000_aps_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_aps_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc3000_aps_v3_t::program_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_sound_number;
        sound_generator_mode_t m_sound_generator_mode;
        uint8_t m_if_over1;
        uint8_t m_use_also_plays1;
        uint8_t m_if_over2;
        uint8_t m_use_also_plays2;
        poly_mode_t m_poly;
        uint8_t m_cutoff_note_1;
        uint8_t m_cutoff_note_2;
        int16_t m_tune;
        uint8_t m_attack;
        uint8_t m_decay;
        decay_mode_t m_decay_mode;
        uint8_t m_filter_frequency;
        uint8_t m_filter_resonance;
        uint8_t m_filter_envel_attack;
        uint8_t m_filter_envel_decay;
        uint8_t m_filter_envel_amount;
        uint8_t m_veloc_mod_of_volume;
        uint8_t m_veloc_mod_of_attack;
        uint8_t m_veloc_mod_of_soft_start;
        uint8_t m_veloc_mod_of_filter_freq;
        note_variation_type_t m_param;
        mpc3000_aps_v3_t* m__root;
        mpc3000_aps_v3_t::program_t* m__parent;
    };

public:

    /**
     * File ID, should be 10
     */
    std::string file_id() const { return m_file_id; }
    void set_file_id(std::string _v) { m__dirty = true; m_file_id = std::move(_v); }
    std::string file_version() const { return m_file_version; }
    void set_file_version(std::string _v) { m__dirty = true; m_file_version = std::move(_v); }

    /**
     * Number of programs in file
     */
    uint8_t num_programs() const { return m_num_programs; }
    void set_num_programs(uint8_t _v) { m__dirty = true; m_num_programs = std::move(_v); }
    uint32_t total_samples() const { return m_total_samples; }
    void set_total_samples(uint32_t _v) { m__dirty = true; m_total_samples = std::move(_v); }
    std::string file_name() const { return m_file_name; }
    void set_file_name(std::string _v) { m__dirty = true; m_file_name = std::move(_v); }
    uint8_t active_program_number() const { return m_active_program_number; }
    void set_active_program_number(uint8_t _v) { m__dirty = true; m_active_program_number = std::move(_v); }
    mix_source_t stereo_mix_source() const { return m_stereo_mix_source; }
    void set_stereo_mix_source(mix_source_t _v) { m__dirty = true; m_stereo_mix_source = std::move(_v); }
    mix_source_t indiv_out_echo_send_mix_source() const { return m_indiv_out_echo_send_mix_source; }
    void set_indiv_out_echo_send_mix_source(mix_source_t _v) { m__dirty = true; m_indiv_out_echo_send_mix_source = std::move(_v); }
    mix_source_t effects_source() const { return m_effects_source; }
    void set_effects_source(mix_source_t _v) { m__dirty = true; m_effects_source = std::move(_v); }
    uint8_t record_live_mix_changes() const { return m_record_live_mix_changes; }
    void set_record_live_mix_changes(uint8_t _v) { m__dirty = true; m_record_live_mix_changes = std::move(_v); }
    uint8_t center_pad_16_levels_if_param_tuning() const { return m_center_pad_16_levels_if_param_tuning; }
    void set_center_pad_16_levels_if_param_tuning(uint8_t _v) { m__dirty = true; m_center_pad_16_levels_if_param_tuning = std::move(_v); }

    /**
     * 35-98 or 0
     */
    uint8_t audio_trigger_assign() const { return m_audio_trigger_assign; }
    void set_audio_trigger_assign(uint8_t _v) { m__dirty = true; m_audio_trigger_assign = std::move(_v); }
    effects_settings_t* effects_settings() const { return m_effects_settings.get(); }
    void set_effects_settings(std::unique_ptr<effects_settings_t> _v) { m__dirty = true; m_effects_settings = std::move(_v); }

    /**
     * Real MPC3000 OS 3.11 empty APS evidence puts a 64-entry top-level mixer table here, despite Roger Linn's note reading like a single master mixer record.
     */
    std::vector<std::unique_ptr<mixer_settings_t>>* mixer_settings() const { return m_mixer_settings.get(); }
    void set_mixer_settings(std::unique_ptr<std::vector<std::unique_ptr<mixer_settings_t>>> _v) { m__dirty = true; m_mixer_settings = std::move(_v); }
    std::vector<std::unique_ptr<program_t>>* programs() const { return m_programs.get(); }
    void set_programs(std::unique_ptr<std::vector<std::unique_ptr<program_t>>> _v) { m__dirty = true; m_programs = std::move(_v); }

    /**
     * Global 128-entry sound-name table. Roger Linn's APS v3 notes place this earlier in the file, but real MPC3000 OS 3.11 empty APS evidence currently points at a trailing table after the program blocks.
     */
    std::vector<std::string>* sound_names() const { return m_sound_names.get(); }
    void set_sound_names(std::unique_ptr<std::vector<std::string>> _v) { m__dirty = true; m_sound_names = std::move(_v); }
    mpc3000_aps_v3_t* _root() const { return m__root; }
    void set__root(mpc3000_aps_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::string m_file_id;
    std::string m_file_version;
    uint8_t m_num_programs;
    uint32_t m_total_samples;
    std::string m_file_name;
    uint8_t m_active_program_number;
    mix_source_t m_stereo_mix_source;
    mix_source_t m_indiv_out_echo_send_mix_source;
    mix_source_t m_effects_source;
    uint8_t m_record_live_mix_changes;
    uint8_t m_center_pad_16_levels_if_param_tuning;
    uint8_t m_audio_trigger_assign;
    std::unique_ptr<effects_settings_t> m_effects_settings;
    std::unique_ptr<std::vector<std::unique_ptr<mixer_settings_t>>> m_mixer_settings;
    std::unique_ptr<std::vector<std::unique_ptr<program_t>>> m_programs;
    std::unique_ptr<std::vector<std::string>> m_sound_names;
    mpc3000_aps_v3_t* m__root;
    kaitai::kstruct* m__parent;
};

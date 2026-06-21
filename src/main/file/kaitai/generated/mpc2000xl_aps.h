#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc2000xl_aps_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include "mpc2000xl_pgm.h"
#include <set>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc2000xl_aps_t : public kaitai::kstruct {

public:
    class aps_program_body_t;
    class aps_program_meta_t;
    class drum_t;
    class global_parameters_t;
    class note_t;

    enum mix_source_t {
        MIX_SOURCE_PROGRAM = 0,
        MIX_SOURCE_DRUM = 1
    };
    static bool _is_defined_mix_source_t(mix_source_t v);

private:
    static const std::set<mix_source_t> _values_mix_source_t;

public:

    enum no_yes_t {
        NO_YES_FALSE = 0,
        NO_YES_TRUE = 1
    };
    static bool _is_defined_no_yes_t(no_yes_t v);

private:
    static const std::set<no_yes_t> _values_no_yes_t;

public:

    enum pad_assign_t {
        PAD_ASSIGN_PROGRAM = 0,
        PAD_ASSIGN_MASTERS = 1
    };
    static bool _is_defined_pad_assign_t(pad_assign_t v);

private:
    static const std::set<pad_assign_t> _values_pad_assign_t;

public:

private:
    bool m__dirty;

public:

    mpc2000xl_aps_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc2000xl_aps_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc2000xl_aps_t();

    class aps_program_body_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        aps_program_body_t(kaitai::kstream* p__io, mpc2000xl_aps_t::aps_program_meta_t* p__parent = nullptr, mpc2000xl_aps_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~aps_program_body_t();
        std::string _unnamed0() const { return m__unnamed0; }
        void set__unnamed0(std::string _v) { m__dirty = true; m__unnamed0 = std::move(_v); }
        std::string name() const { return m_name; }
        void set_name(std::string _v) { m__dirty = true; m_name = std::move(_v); }
        mpc2000xl_pgm_t::slider_t* slider() const { return m_slider.get(); }
        void set_slider(std::unique_ptr<mpc2000xl_pgm_t::slider_t> _v) { m__dirty = true; m_slider = std::move(_v); }
        uint8_t program_change() const { return m_program_change; }
        void set_program_change(uint8_t _v) { m__dirty = true; m_program_change = std::move(_v); }
        std::string _unnamed4() const { return m__unnamed4; }
        void set__unnamed4(std::string _v) { m__dirty = true; m__unnamed4 = std::move(_v); }
        std::vector<std::unique_ptr<note_t>>* note_parameters() const { return m_note_parameters.get(); }
        void set_note_parameters(std::unique_ptr<std::vector<std::unique_ptr<note_t>>> _v) { m__dirty = true; m_note_parameters = std::move(_v); }
        std::string _unnamed6() const { return m__unnamed6; }
        void set__unnamed6(std::string _v) { m__dirty = true; m__unnamed6 = std::move(_v); }
        std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>* pad_mixers() const { return m_pad_mixers.get(); }
        void set_pad_mixers(std::unique_ptr<std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>> _v) { m__dirty = true; m_pad_mixers = std::move(_v); }
        std::string _unnamed8() const { return m__unnamed8; }
        void set__unnamed8(std::string _v) { m__dirty = true; m__unnamed8 = std::move(_v); }
        std::vector<int8_t>* pad_to_note_mapping() const { return m_pad_to_note_mapping.get(); }
        void set_pad_to_note_mapping(std::unique_ptr<std::vector<int8_t>> _v) { m__dirty = true; m_pad_to_note_mapping = std::move(_v); }
        std::string _unnamed10() const { return m__unnamed10; }
        void set__unnamed10(std::string _v) { m__dirty = true; m__unnamed10 = std::move(_v); }
        mpc2000xl_aps_t* _root() const { return m__root; }
        void set__root(mpc2000xl_aps_t* _v) { m__dirty = true; m__root = std::move(_v); }
        mpc2000xl_aps_t::aps_program_meta_t* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_aps_t::aps_program_meta_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m__unnamed0;
        std::string m_name;
        std::unique_ptr<mpc2000xl_pgm_t::slider_t> m_slider;
        uint8_t m_program_change;
        std::string m__unnamed4;
        std::unique_ptr<std::vector<std::unique_ptr<note_t>>> m_note_parameters;
        std::string m__unnamed6;
        std::unique_ptr<std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>> m_pad_mixers;
        std::string m__unnamed8;
        std::unique_ptr<std::vector<int8_t>> m_pad_to_note_mapping;
        std::string m__unnamed10;
        mpc2000xl_aps_t* m__root;
        mpc2000xl_aps_t::aps_program_meta_t* m__parent;
    };

    class aps_program_meta_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        aps_program_meta_t(kaitai::kstream* p__io, mpc2000xl_aps_t* p__parent = nullptr, mpc2000xl_aps_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~aps_program_meta_t();
        uint8_t index() const { return m_index; }
        void set_index(uint8_t _v) { m__dirty = true; m_index = std::move(_v); }
        aps_program_body_t* body() const { return m_body.get(); }
        void set_body(std::unique_ptr<aps_program_body_t> _v) { m__dirty = true; m_body = std::move(_v); }
        std::vector<uint8_t>* _unnamed2() const { return m__unnamed2.get(); }
        void set__unnamed2(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        mpc2000xl_aps_t* _root() const { return m__root; }
        void set__root(mpc2000xl_aps_t* _v) { m__dirty = true; m__root = std::move(_v); }
        mpc2000xl_aps_t* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_aps_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_index;
        std::unique_ptr<aps_program_body_t> m_body;

    public:
        bool _is_null_body() { return !body(); };

    private:
        std::unique_ptr<std::vector<uint8_t>> m__unnamed2;

    public:
        bool _is_null__unnamed2() { return !_unnamed2(); };

    private:
        mpc2000xl_aps_t* m__root;
        mpc2000xl_aps_t* m__parent;
    };

    class drum_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        drum_t(kaitai::kstream* p__io, mpc2000xl_aps_t* p__parent = nullptr, mpc2000xl_aps_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~drum_t();
        std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>* pad_mixers() const { return m_pad_mixers.get(); }
        void set_pad_mixers(std::unique_ptr<std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>> _v) { m__dirty = true; m_pad_mixers = std::move(_v); }
        std::string _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(std::string _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        no_yes_t receive_program_change() const { return m_receive_program_change; }
        void set_receive_program_change(no_yes_t _v) { m__dirty = true; m_receive_program_change = std::move(_v); }
        no_yes_t receive_midi_volume() const { return m_receive_midi_volume; }
        void set_receive_midi_volume(no_yes_t _v) { m__dirty = true; m_receive_midi_volume = std::move(_v); }
        std::string _unnamed4() const { return m__unnamed4; }
        void set__unnamed4(std::string _v) { m__dirty = true; m__unnamed4 = std::move(_v); }
        uint8_t program() const { return m_program; }
        void set_program(uint8_t _v) { m__dirty = true; m_program = std::move(_v); }
        no_yes_t receive_program_change_duplicate() const { return m_receive_program_change_duplicate; }
        void set_receive_program_change_duplicate(no_yes_t _v) { m__dirty = true; m_receive_program_change_duplicate = std::move(_v); }
        no_yes_t receive_midi_volume_duplicate() const { return m_receive_midi_volume_duplicate; }
        void set_receive_midi_volume_duplicate(no_yes_t _v) { m__dirty = true; m_receive_midi_volume_duplicate = std::move(_v); }
        mpc2000xl_aps_t* _root() const { return m__root; }
        void set__root(mpc2000xl_aps_t* _v) { m__dirty = true; m__root = std::move(_v); }
        mpc2000xl_aps_t* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_aps_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::unique_ptr<std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>> m_pad_mixers;
        std::string m__unnamed1;
        no_yes_t m_receive_program_change;
        no_yes_t m_receive_midi_volume;
        std::string m__unnamed4;
        uint8_t m_program;
        no_yes_t m_receive_program_change_duplicate;
        no_yes_t m_receive_midi_volume_duplicate;
        mpc2000xl_aps_t* m__root;
        mpc2000xl_aps_t* m__parent;
    };

    class global_parameters_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        global_parameters_t(kaitai::kstream* p__io, mpc2000xl_aps_t* p__parent = nullptr, mpc2000xl_aps_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~global_parameters_t();
        no_yes_t pad_to_internal_sound() const { return m_pad_to_internal_sound; }
        void set_pad_to_internal_sound(no_yes_t _v) { m__dirty = true; m_pad_to_internal_sound = std::move(_v); }
        uint64_t _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(uint64_t _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        pad_assign_t pad_assign() const { return m_pad_assign; }
        void set_pad_assign(pad_assign_t _v) { m__dirty = true; m_pad_assign = std::move(_v); }
        uint64_t _unnamed3() const { return m__unnamed3; }
        void set__unnamed3(uint64_t _v) { m__dirty = true; m__unnamed3 = std::move(_v); }
        mix_source_t stereo_mix_source() const { return m_stereo_mix_source; }
        void set_stereo_mix_source(mix_source_t _v) { m__dirty = true; m_stereo_mix_source = std::move(_v); }
        mix_source_t indiv_fx_source() const { return m_indiv_fx_source; }
        void set_indiv_fx_source(mix_source_t _v) { m__dirty = true; m_indiv_fx_source = std::move(_v); }
        uint64_t _unnamed6() const { return m__unnamed6; }
        void set__unnamed6(uint64_t _v) { m__dirty = true; m__unnamed6 = std::move(_v); }
        no_yes_t copy_pgm_mix_to_drum() const { return m_copy_pgm_mix_to_drum; }
        void set_copy_pgm_mix_to_drum(no_yes_t _v) { m__dirty = true; m_copy_pgm_mix_to_drum = std::move(_v); }
        uint64_t _unnamed8() const { return m__unnamed8; }
        void set__unnamed8(uint64_t _v) { m__dirty = true; m__unnamed8 = std::move(_v); }
        no_yes_t record_mix_changes() const { return m_record_mix_changes; }
        void set_record_mix_changes(no_yes_t _v) { m__dirty = true; m_record_mix_changes = std::move(_v); }
        uint64_t _unnamed10() const { return m__unnamed10; }
        void set__unnamed10(uint64_t _v) { m__dirty = true; m__unnamed10 = std::move(_v); }
        uint8_t fx_drum() const { return m_fx_drum; }
        void set_fx_drum(uint8_t _v) { m__dirty = true; m_fx_drum = std::move(_v); }
        std::string _unnamed12() const { return m__unnamed12; }
        void set__unnamed12(std::string _v) { m__dirty = true; m__unnamed12 = std::move(_v); }
        uint8_t master_level() const { return m_master_level; }
        void set_master_level(uint8_t _v) { m__dirty = true; m_master_level = std::move(_v); }
        mpc2000xl_aps_t* _root() const { return m__root; }
        void set__root(mpc2000xl_aps_t* _v) { m__dirty = true; m__root = std::move(_v); }
        mpc2000xl_aps_t* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_aps_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        no_yes_t m_pad_to_internal_sound;
        uint64_t m__unnamed1;
        pad_assign_t m_pad_assign;
        uint64_t m__unnamed3;
        mix_source_t m_stereo_mix_source;
        mix_source_t m_indiv_fx_source;
        uint64_t m__unnamed6;
        no_yes_t m_copy_pgm_mix_to_drum;
        uint64_t m__unnamed8;
        no_yes_t m_record_mix_changes;
        uint64_t m__unnamed10;
        uint8_t m_fx_drum;
        std::string m__unnamed12;
        uint8_t m_master_level;
        mpc2000xl_aps_t* m__root;
        mpc2000xl_aps_t* m__parent;
    };

    class note_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        note_t(kaitai::kstream* p__io, mpc2000xl_aps_t::aps_program_body_t* p__parent = nullptr, mpc2000xl_aps_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~note_t();
        uint8_t sound_index() const { return m_sound_index; }
        void set_sound_index(uint8_t _v) { m__dirty = true; m_sound_index = std::move(_v); }
        std::string _unnamed1() const { return m__unnamed1; }
        void set__unnamed1(std::string _v) { m__dirty = true; m__unnamed1 = std::move(_v); }
        mpc2000xl_pgm_t::sound_generation_mode_t sound_generation_mode() const { return m_sound_generation_mode; }
        void set_sound_generation_mode(mpc2000xl_pgm_t::sound_generation_mode_t _v) { m__dirty = true; m_sound_generation_mode = std::move(_v); }
        uint8_t velocity_range_lower() const { return m_velocity_range_lower; }
        void set_velocity_range_lower(uint8_t _v) { m__dirty = true; m_velocity_range_lower = std::move(_v); }
        uint8_t also_play_use_note_1() const { return m_also_play_use_note_1; }
        void set_also_play_use_note_1(uint8_t _v) { m__dirty = true; m_also_play_use_note_1 = std::move(_v); }
        uint8_t velocity_range_upper() const { return m_velocity_range_upper; }
        void set_velocity_range_upper(uint8_t _v) { m__dirty = true; m_velocity_range_upper = std::move(_v); }
        uint8_t also_play_use_note_2() const { return m_also_play_use_note_2; }
        void set_also_play_use_note_2(uint8_t _v) { m__dirty = true; m_also_play_use_note_2 = std::move(_v); }
        mpc2000xl_pgm_t::voice_overlap_mode_t voice_overlap_mode() const { return m_voice_overlap_mode; }
        void set_voice_overlap_mode(mpc2000xl_pgm_t::voice_overlap_mode_t _v) { m__dirty = true; m_voice_overlap_mode = std::move(_v); }
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
        mpc2000xl_pgm_t::decay_mode_t decay_mode() const { return m_decay_mode; }
        void set_decay_mode(mpc2000xl_pgm_t::decay_mode_t _v) { m__dirty = true; m_decay_mode = std::move(_v); }
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
        mpc2000xl_pgm_t::slider_parameter_t slider_parameter() const { return m_slider_parameter; }
        void set_slider_parameter(mpc2000xl_pgm_t::slider_parameter_t _v) { m__dirty = true; m_slider_parameter = std::move(_v); }
        uint8_t velocity_to_pitch() const { return m_velocity_to_pitch; }
        void set_velocity_to_pitch(uint8_t _v) { m__dirty = true; m_velocity_to_pitch = std::move(_v); }
        mpc2000xl_aps_t* _root() const { return m__root; }
        void set__root(mpc2000xl_aps_t* _v) { m__dirty = true; m__root = std::move(_v); }
        mpc2000xl_aps_t::aps_program_body_t* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_aps_t::aps_program_body_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_sound_index;
        std::string m__unnamed1;
        mpc2000xl_pgm_t::sound_generation_mode_t m_sound_generation_mode;
        uint8_t m_velocity_range_lower;
        uint8_t m_also_play_use_note_1;
        uint8_t m_velocity_range_upper;
        uint8_t m_also_play_use_note_2;
        mpc2000xl_pgm_t::voice_overlap_mode_t m_voice_overlap_mode;
        uint8_t m_mute_assign_1;
        uint8_t m_mute_assign_2;
        int16_t m_tune;
        uint8_t m_attack;
        uint8_t m_decay;
        mpc2000xl_pgm_t::decay_mode_t m_decay_mode;
        uint8_t m_cutoff;
        uint8_t m_resonance;
        uint8_t m_velocity_envelope_to_filter_attack;
        uint8_t m_velocity_envelope_to_filter_decay;
        uint8_t m_velocity_envelope_to_filter_amount;
        uint8_t m_velocity_to_level;
        uint8_t m_velocity_to_attack;
        uint8_t m_velocity_to_start;
        uint8_t m_velocity_to_cutoff;
        mpc2000xl_pgm_t::slider_parameter_t m_slider_parameter;
        uint8_t m_velocity_to_pitch;
        mpc2000xl_aps_t* m__root;
        mpc2000xl_aps_t::aps_program_body_t* m__parent;
    };

public:
    std::string magic() const { return m_magic; }
    void set_magic(std::string _v) { m__dirty = true; m_magic = std::move(_v); }
    uint16_t sound_count() const { return m_sound_count; }
    void set_sound_count(uint16_t _v) { m__dirty = true; m_sound_count = std::move(_v); }
    std::vector<std::string>* sound_names() const { return m_sound_names.get(); }
    void set_sound_names(std::unique_ptr<std::vector<std::string>> _v) { m__dirty = true; m_sound_names = std::move(_v); }
    std::string _unnamed4() const { return m__unnamed4; }
    void set__unnamed4(std::string _v) { m__dirty = true; m__unnamed4 = std::move(_v); }
    std::string name() const { return m_name; }
    void set_name(std::string _v) { m__dirty = true; m_name = std::move(_v); }
    global_parameters_t* global_parameters() const { return m_global_parameters.get(); }
    void set_global_parameters(std::unique_ptr<global_parameters_t> _v) { m__dirty = true; m_global_parameters = std::move(_v); }
    std::string _unnamed7() const { return m__unnamed7; }
    void set__unnamed7(std::string _v) { m__dirty = true; m__unnamed7 = std::move(_v); }
    std::vector<int8_t>* master_pad_to_note_mapping() const { return m_master_pad_to_note_mapping.get(); }
    void set_master_pad_to_note_mapping(std::unique_ptr<std::vector<int8_t>> _v) { m__dirty = true; m_master_pad_to_note_mapping = std::move(_v); }
    std::string _unnamed9() const { return m__unnamed9; }
    void set__unnamed9(std::string _v) { m__dirty = true; m__unnamed9 = std::move(_v); }
    drum_t* drum1() const { return m_drum1.get(); }
    void set_drum1(std::unique_ptr<drum_t> _v) { m__dirty = true; m_drum1 = std::move(_v); }
    std::string _unnamed11() const { return m__unnamed11; }
    void set__unnamed11(std::string _v) { m__dirty = true; m__unnamed11 = std::move(_v); }
    drum_t* drum2() const { return m_drum2.get(); }
    void set_drum2(std::unique_ptr<drum_t> _v) { m__dirty = true; m_drum2 = std::move(_v); }
    std::string _unnamed13() const { return m__unnamed13; }
    void set__unnamed13(std::string _v) { m__dirty = true; m__unnamed13 = std::move(_v); }
    drum_t* drum3() const { return m_drum3.get(); }
    void set_drum3(std::unique_ptr<drum_t> _v) { m__dirty = true; m_drum3 = std::move(_v); }
    std::string _unnamed15() const { return m__unnamed15; }
    void set__unnamed15(std::string _v) { m__dirty = true; m__unnamed15 = std::move(_v); }
    drum_t* drum4() const { return m_drum4.get(); }
    void set_drum4(std::unique_ptr<drum_t> _v) { m__dirty = true; m_drum4 = std::move(_v); }
    std::string _unnamed17() const { return m__unnamed17; }
    void set__unnamed17(std::string _v) { m__dirty = true; m__unnamed17 = std::move(_v); }
    std::vector<std::unique_ptr<aps_program_meta_t>>* aps_programs() const { return m_aps_programs.get(); }
    void set_aps_programs(std::unique_ptr<std::vector<std::unique_ptr<aps_program_meta_t>>> _v) { m__dirty = true; m_aps_programs = std::move(_v); }
    mpc2000xl_aps_t* _root() const { return m__root; }
    void set__root(mpc2000xl_aps_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::string m_magic;
    uint16_t m_sound_count;
    std::unique_ptr<std::vector<std::string>> m_sound_names;
    std::string m__unnamed4;
    std::string m_name;
    std::unique_ptr<global_parameters_t> m_global_parameters;
    std::string m__unnamed7;
    std::unique_ptr<std::vector<int8_t>> m_master_pad_to_note_mapping;
    std::string m__unnamed9;
    std::unique_ptr<drum_t> m_drum1;
    std::string m__unnamed11;
    std::unique_ptr<drum_t> m_drum2;
    std::string m__unnamed13;
    std::unique_ptr<drum_t> m_drum3;
    std::string m__unnamed15;
    std::unique_ptr<drum_t> m_drum4;
    std::string m__unnamed17;
    std::unique_ptr<std::vector<std::unique_ptr<aps_program_meta_t>>> m_aps_programs;
    mpc2000xl_aps_t* m__root;
    kaitai::kstruct* m__parent;
};

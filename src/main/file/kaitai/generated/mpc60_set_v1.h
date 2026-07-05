#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc60_set_v1_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <set>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc60_set_v1_t : public kaitai::kstruct {

public:
    class sound_directory_entry_t;
    class u3le_t;

    enum analog_output_jack_t {
        ANALOG_OUTPUT_JACK_FALSE = 7,
        ANALOG_OUTPUT_JACK_JACK_1 = 8,
        ANALOG_OUTPUT_JACK_JACK_2 = 9,
        ANALOG_OUTPUT_JACK_JACK_3 = 10,
        ANALOG_OUTPUT_JACK_JACK_4 = 11,
        ANALOG_OUTPUT_JACK_JACK_5 = 12,
        ANALOG_OUTPUT_JACK_JACK_6 = 13,
        ANALOG_OUTPUT_JACK_JACK_7 = 14,
        ANALOG_OUTPUT_JACK_JACK_8 = 15
    };
    static bool _is_defined_analog_output_jack_t(analog_output_jack_t v);

private:
    static const std::set<analog_output_jack_t> _values_analog_output_jack_t;

public:

    enum filter_range_t {
        FILTER_RANGE_LESS_THAN_OR_EQUALS_47_KHZ = 0,
        FILTER_RANGE_GREATER_THAN_47_KHZ = 1
    };
    static bool _is_defined_filter_range_t(filter_range_t v);

private:
    static const std::set<filter_range_t> _values_filter_range_t;

public:

    enum normal_or_hihat_t {
        NORMAL_OR_HIHAT_NORMAL = 0,
        NORMAL_OR_HIHAT_HIHAT = 1
    };
    static bool _is_defined_normal_or_hihat_t(normal_or_hihat_t v);

private:
    static const std::set<normal_or_hihat_t> _values_normal_or_hihat_t;

public:

    enum physical_drum_t {
        PHYSICAL_DRUM_HIHT_CLSD = 0,
        PHYSICAL_DRUM_HIHT_MEDM = 1,
        PHYSICAL_DRUM_HIHT_OPEN = 2,
        PHYSICAL_DRUM_SNR1 = 3,
        PHYSICAL_DRUM_SNR2 = 4,
        PHYSICAL_DRUM_BASS = 5,
        PHYSICAL_DRUM_TOM1 = 6,
        PHYSICAL_DRUM_TOM2 = 7,
        PHYSICAL_DRUM_TOM3 = 8,
        PHYSICAL_DRUM_TOM4 = 9,
        PHYSICAL_DRUM_RID1 = 10,
        PHYSICAL_DRUM_RID2 = 11,
        PHYSICAL_DRUM_CRS1 = 12,
        PHYSICAL_DRUM_CRS2 = 13,
        PHYSICAL_DRUM_PRC1 = 14,
        PHYSICAL_DRUM_PRC2 = 15,
        PHYSICAL_DRUM_PRC3 = 16,
        PHYSICAL_DRUM_PRC4 = 17,
        PHYSICAL_DRUM_DR01 = 18,
        PHYSICAL_DRUM_DR02 = 19,
        PHYSICAL_DRUM_DR03 = 20,
        PHYSICAL_DRUM_DR04 = 21,
        PHYSICAL_DRUM_DR05 = 22,
        PHYSICAL_DRUM_DR06 = 23,
        PHYSICAL_DRUM_DR07 = 24,
        PHYSICAL_DRUM_DR08 = 25,
        PHYSICAL_DRUM_DR09 = 26,
        PHYSICAL_DRUM_DR10 = 27,
        PHYSICAL_DRUM_DR11 = 28,
        PHYSICAL_DRUM_DR12 = 29,
        PHYSICAL_DRUM_DR13 = 30,
        PHYSICAL_DRUM_DR14 = 31,
        PHYSICAL_DRUM_DR15 = 32,
        PHYSICAL_DRUM_DR16 = 33
    };
    static bool _is_defined_physical_drum_t(physical_drum_t v);

private:
    static const std::set<physical_drum_t> _values_physical_drum_t;

public:

    enum use_master_mix_data_t {
        USE_MASTER_MIX_DATA_IGNORE = 0,
        USE_MASTER_MIX_DATA_USE = 1
    };
    static bool _is_defined_use_master_mix_data_t(use_master_mix_data_t v);

private:
    static const std::set<use_master_mix_data_t> _values_use_master_mix_data_t;

public:

private:
    bool m__dirty;

public:

    mpc60_set_v1_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc60_set_v1_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc60_set_v1_t();

    class sound_directory_entry_t : public kaitai::kstruct {

    public:
        class sound_characteristics_t;

    private:
        bool m__dirty;

    public:

        sound_directory_entry_t(kaitai::kstream* p__io, mpc60_set_v1_t* p__parent = nullptr, mpc60_set_v1_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sound_directory_entry_t();

        class sound_characteristics_t : public kaitai::kstruct {

        public:

        private:
            bool m__dirty;

        public:

            sound_characteristics_t(kaitai::kstream* p__io, mpc60_set_v1_t::sound_directory_entry_t* p__parent = nullptr, mpc60_set_v1_t* p__root = nullptr);
            void _read();
            void _fetch_instances();
            void _write();
            void _check();
            ~sound_characteristics_t();
            normal_or_hihat_t normal_or_hihat_sound() const { return m_normal_or_hihat_sound; }
            void set_normal_or_hihat_sound(normal_or_hihat_t _v) { m__dirty = true; m_normal_or_hihat_sound = std::move(_v); }
            bool save_required() const { return m_save_required; }
            void set_save_required(bool _v) { m__dirty = true; m_save_required = std::move(_v); }
            uint64_t _unnamed2() const { return m__unnamed2; }
            void set__unnamed2(uint64_t _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
            filter_range_t filter() const { return m_filter; }
            void set_filter(filter_range_t _v) { m__dirty = true; m_filter = std::move(_v); }
            mpc60_set_v1_t* _root() const { return m__root; }
            void set__root(mpc60_set_v1_t* _v) { m__dirty = true; m__root = std::move(_v); }
            kaitai::kstruct* _parent() const { return m__parent; }
            void set__parent(mpc60_set_v1_t::sound_directory_entry_t* _v) { m__dirty = true; m__parent = std::move(_v); }

        private:
            normal_or_hihat_t m_normal_or_hihat_sound;
            bool m_save_required;
            uint64_t m__unnamed2;
            filter_range_t m_filter;
            mpc60_set_v1_t* m__root;
            mpc60_set_v1_t::sound_directory_entry_t* m__parent;
        };

    public:
        std::string name() const { return m_name; }
        void set_name(std::string _v) { m__dirty = true; m_name = std::move(_v); }
        uint8_t shared_sound_link() const { return m_shared_sound_link; }
        void set_shared_sound_link(uint8_t _v) { m__dirty = true; m_shared_sound_link = std::move(_v); }
        uint32_t start_address_in_memory() const { return m_start_address_in_memory; }
        void set_start_address_in_memory(uint32_t _v) { m__dirty = true; m_start_address_in_memory = std::move(_v); }
        uint32_t length_in_samples() const { return m_length_in_samples; }
        void set_length_in_samples(uint32_t _v) { m__dirty = true; m_length_in_samples = std::move(_v); }
        uint32_t read_write_pointer() const { return m_read_write_pointer; }
        void set_read_write_pointer(uint32_t _v) { m__dirty = true; m_read_write_pointer = std::move(_v); }
        uint32_t start_address_for_playing() const { return m_start_address_for_playing; }
        void set_start_address_for_playing(uint32_t _v) { m__dirty = true; m_start_address_for_playing = std::move(_v); }
        uint16_t time_from_start_of_data_to_start_play_msec() const { return m_time_from_start_of_data_to_start_play_msec; }
        void set_time_from_start_of_data_to_start_play_msec(uint16_t _v) { m__dirty = true; m_time_from_start_of_data_to_start_play_msec = std::move(_v); }
        uint16_t time_from_start_of_data_to_end_of_play_msec() const { return m_time_from_start_of_data_to_end_of_play_msec; }
        void set_time_from_start_of_data_to_end_of_play_msec(uint16_t _v) { m__dirty = true; m_time_from_start_of_data_to_end_of_play_msec = std::move(_v); }
        uint16_t attack_time_msec() const { return m_attack_time_msec; }
        void set_attack_time_msec(uint16_t _v) { m__dirty = true; m_attack_time_msec = std::move(_v); }
        uint16_t decay_time_msec() const { return m_decay_time_msec; }
        void set_decay_time_msec(uint16_t _v) { m__dirty = true; m_decay_time_msec = std::move(_v); }
        uint16_t pitch_factor() const { return m_pitch_factor; }
        void set_pitch_factor(uint16_t _v) { m__dirty = true; m_pitch_factor = std::move(_v); }
        uint16_t attack_rate_sent_to_sound_generator() const { return m_attack_rate_sent_to_sound_generator; }
        void set_attack_rate_sent_to_sound_generator(uint16_t _v) { m__dirty = true; m_attack_rate_sent_to_sound_generator = std::move(_v); }
        uint16_t decay_rate_sent_to_sound_generator() const { return m_decay_rate_sent_to_sound_generator; }
        void set_decay_rate_sent_to_sound_generator(uint16_t _v) { m__dirty = true; m_decay_rate_sent_to_sound_generator = std::move(_v); }
        uint16_t decay_start_time() const { return m_decay_start_time; }
        void set_decay_start_time(uint16_t _v) { m__dirty = true; m_decay_start_time = std::move(_v); }
        uint16_t sound_duration() const { return m_sound_duration; }
        void set_sound_duration(uint16_t _v) { m__dirty = true; m_sound_duration = std::move(_v); }
        analog_output_jack_t analog_output_jack() const { return m_analog_output_jack; }
        void set_analog_output_jack(analog_output_jack_t _v) { m__dirty = true; m_analog_output_jack = std::move(_v); }
        uint8_t requested_stereo_mix_volume() const { return m_requested_stereo_mix_volume; }
        void set_requested_stereo_mix_volume(uint8_t _v) { m__dirty = true; m_requested_stereo_mix_volume = std::move(_v); }
        uint8_t requested_stereo_mix_pan() const { return m_requested_stereo_mix_pan; }
        void set_requested_stereo_mix_pan(uint8_t _v) { m__dirty = true; m_requested_stereo_mix_pan = std::move(_v); }
        uint8_t left_level_sent_to_sound_generator() const { return m_left_level_sent_to_sound_generator; }
        void set_left_level_sent_to_sound_generator(uint8_t _v) { m__dirty = true; m_left_level_sent_to_sound_generator = std::move(_v); }
        uint8_t right_level_sent_to_sound_generator() const { return m_right_level_sent_to_sound_generator; }
        void set_right_level_sent_to_sound_generator(uint8_t _v) { m__dirty = true; m_right_level_sent_to_sound_generator = std::move(_v); }
        uint8_t echo_level_sent_to_sound_generator() const { return m_echo_level_sent_to_sound_generator; }
        void set_echo_level_sent_to_sound_generator(uint8_t _v) { m__dirty = true; m_echo_level_sent_to_sound_generator = std::move(_v); }
        sound_characteristics_t* sound_characteristics() const { return m_sound_characteristics.get(); }
        void set_sound_characteristics(std::unique_ptr<sound_characteristics_t> _v) { m__dirty = true; m_sound_characteristics = std::move(_v); }
        mpc60_set_v1_t* _root() const { return m__root; }
        void set__root(mpc60_set_v1_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc60_set_v1_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m_name;
        uint8_t m_shared_sound_link;
        uint32_t m_start_address_in_memory;
        uint32_t m_length_in_samples;
        uint32_t m_read_write_pointer;
        uint32_t m_start_address_for_playing;
        uint16_t m_time_from_start_of_data_to_start_play_msec;
        uint16_t m_time_from_start_of_data_to_end_of_play_msec;
        uint16_t m_attack_time_msec;
        uint16_t m_decay_time_msec;
        uint16_t m_pitch_factor;
        uint16_t m_attack_rate_sent_to_sound_generator;
        uint16_t m_decay_rate_sent_to_sound_generator;
        uint16_t m_decay_start_time;
        uint16_t m_sound_duration;
        analog_output_jack_t m_analog_output_jack;
        uint8_t m_requested_stereo_mix_volume;
        uint8_t m_requested_stereo_mix_pan;
        uint8_t m_left_level_sent_to_sound_generator;
        uint8_t m_right_level_sent_to_sound_generator;
        uint8_t m_echo_level_sent_to_sound_generator;
        std::unique_ptr<sound_characteristics_t> m_sound_characteristics;
        mpc60_set_v1_t* m__root;
        mpc60_set_v1_t* m__parent;
    };

    class u3le_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        u3le_t(kaitai::kstream* p__io, mpc60_set_v1_t* p__parent = nullptr, mpc60_set_v1_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~u3le_t();
        int32_t value();
        void _invalidate_value() { f_value = false; }
        uint16_t b12() const { return m_b12; }
        void set_b12(uint16_t _v) { m__dirty = true; f_value = false; m_b12 = std::move(_v); }
        uint8_t b3() const { return m_b3; }
        void set_b3(uint8_t _v) { m__dirty = true; f_value = false; m_b3 = std::move(_v); }
        mpc60_set_v1_t* _root() const { return m__root; }
        void set__root(mpc60_set_v1_t* _v) { m__dirty = true; f_value = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc60_set_v1_t* _v) { m__dirty = true; f_value = false; m__parent = std::move(_v); }

    private:
        bool f_value;
        int32_t m_value;
        uint16_t m_b12;
        uint8_t m_b3;
        mpc60_set_v1_t* m__root;
        mpc60_set_v1_t* m__parent;
    };

public:
    std::string file_id() const { return m_file_id; }
    void set_file_id(std::string _v) { m__dirty = true; m_file_id = std::move(_v); }
    std::string file_format_version() const { return m_file_format_version; }
    void set_file_format_version(std::string _v) { m__dirty = true; m_file_format_version = std::move(_v); }
    u3le_t* total_number_of_sample_words() const { return m_total_number_of_sample_words.get(); }
    void set_total_number_of_sample_words(std::unique_ptr<u3le_t> _v) { m__dirty = true; m_total_number_of_sample_words = std::move(_v); }
    std::vector<std::unique_ptr<sound_directory_entry_t>>* sound_directory_entry() const { return m_sound_directory_entry.get(); }
    void set_sound_directory_entry(std::unique_ptr<std::vector<std::unique_ptr<sound_directory_entry_t>>> _v) { m__dirty = true; m_sound_directory_entry = std::move(_v); }
    std::vector<uint8_t>* sound_map() const { return m_sound_map.get(); }
    void set_sound_map(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_sound_map = std::move(_v); }
    use_master_mix_data_t use_master_mix_data() const { return m_use_master_mix_data; }
    void set_use_master_mix_data(use_master_mix_data_t _v) { m__dirty = true; m_use_master_mix_data = std::move(_v); }
    std::string master_stereo_mix() const { return m_master_stereo_mix; }
    void set_master_stereo_mix(std::string _v) { m__dirty = true; m_master_stereo_mix = std::move(_v); }
    std::string master_stereo_pan() const { return m_master_stereo_pan; }
    void set_master_stereo_pan(std::string _v) { m__dirty = true; m_master_stereo_pan = std::move(_v); }
    std::string master_echo_mix() const { return m_master_echo_mix; }
    void set_master_echo_mix(std::string _v) { m__dirty = true; m_master_echo_mix = std::move(_v); }
    std::string master_drum_tuning() const { return m_master_drum_tuning; }
    void set_master_drum_tuning(std::string _v) { m__dirty = true; m_master_drum_tuning = std::move(_v); }
    std::string double_play_assignments() const { return m_double_play_assignments; }
    void set_double_play_assignments(std::string _v) { m__dirty = true; m_double_play_assignments = std::move(_v); }
    std::string velocity_switch_on_off_for_each_double_play_assign() const { return m_velocity_switch_on_off_for_each_double_play_assign; }
    void set_velocity_switch_on_off_for_each_double_play_assign(std::string _v) { m__dirty = true; m_velocity_switch_on_off_for_each_double_play_assign = std::move(_v); }
    std::string velocity_switch_threshold_value_for_each_double_play_assign() const { return m_velocity_switch_threshold_value_for_each_double_play_assign; }
    void set_velocity_switch_threshold_value_for_each_double_play_assign(std::string _v) { m__dirty = true; m_velocity_switch_threshold_value_for_each_double_play_assign = std::move(_v); }
    std::string _unnamed13() const { return m__unnamed13; }
    void set__unnamed13(std::string _v) { m__dirty = true; m__unnamed13 = std::move(_v); }
    std::vector<uint64_t>* sound_samples() const { return m_sound_samples.get(); }
    void set_sound_samples(std::unique_ptr<std::vector<uint64_t>> _v) { m__dirty = true; m_sound_samples = std::move(_v); }
    mpc60_set_v1_t* _root() const { return m__root; }
    void set__root(mpc60_set_v1_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::string m_file_id;
    std::string m_file_format_version;
    std::unique_ptr<u3le_t> m_total_number_of_sample_words;
    std::unique_ptr<std::vector<std::unique_ptr<sound_directory_entry_t>>> m_sound_directory_entry;
    std::unique_ptr<std::vector<uint8_t>> m_sound_map;
    use_master_mix_data_t m_use_master_mix_data;
    std::string m_master_stereo_mix;
    std::string m_master_stereo_pan;
    std::string m_master_echo_mix;
    std::string m_master_drum_tuning;
    std::string m_double_play_assignments;
    std::string m_velocity_switch_on_off_for_each_double_play_assign;
    std::string m_velocity_switch_threshold_value_for_each_double_play_assign;
    std::string m__unnamed13;
    std::unique_ptr<std::vector<uint64_t>> m_sound_samples;
    mpc60_set_v1_t* m__root;
    kaitai::kstruct* m__parent;
};

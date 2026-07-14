#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc60_seq_body_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include "mpc3000_seq_v3.h"
#include <set>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc60_seq_body_t : public kaitai::kstruct {

public:
    class sequence_t;
    class sequence_header_t;
    class smpte_offset_t;
    class u3le_t;

    enum off_on_t {
        OFF_ON_FALSE = 0,
        OFF_ON_TRUE = 1
    };
    static bool _is_defined_off_on_t(off_on_t v);

private:
    static const std::set<off_on_t> _values_off_on_t;

public:

private:
    bool m__dirty;

public:

    mpc60_seq_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc60_seq_body_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc60_seq_body_t();

    class sequence_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sequence_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc60_seq_body_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequence_t();
        int32_t events_start();
        void _invalidate_events_start() { f_events_start = false; }
        sequence_header_t* sequence_header() const { return m_sequence_header.get(); }
        void set_sequence_header(std::unique_ptr<sequence_header_t> _v) { m__dirty = true; f_events_start = false; m_sequence_header = std::move(_v); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>* track_headers() const { return m_track_headers.get(); }
        void set_track_headers(std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>> _v) { m__dirty = true; f_events_start = false; m_track_headers = std::move(_v); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>* tempo_changes() const { return m_tempo_changes.get(); }
        void set_tempo_changes(std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>> _v) { m__dirty = true; f_events_start = false; m_tempo_changes = std::move(_v); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>* events() const { return m_events.get(); }
        void set_events(std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>> _v) { m__dirty = true; f_events_start = false; m_events = std::move(_v); }
        mpc60_seq_body_t* _root() const { return m__root; }
        void set__root(mpc60_seq_body_t* _v) { m__dirty = true; f_events_start = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(kaitai::kstruct* _v) { m__dirty = true; f_events_start = false; m__parent = std::move(_v); }

    private:
        bool f_events_start;
        int32_t m_events_start;
        std::unique_ptr<sequence_header_t> m_sequence_header;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>> m_track_headers;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>> m_tempo_changes;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>> m_events;
        mpc60_seq_body_t* m__root;
        kaitai::kstruct* m__parent;
    };

    class sequence_header_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sequence_header_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_t* p__parent = nullptr, mpc60_seq_body_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequence_header_t();
        uint8_t sequence_number() const { return m_sequence_number; }
        void set_sequence_number(uint8_t _v) { m__dirty = true; m_sequence_number = std::move(_v); }
        u3le_t* event_stream_length_in_bytes() const { return m_event_stream_length_in_bytes.get(); }
        void set_event_stream_length_in_bytes(std::unique_ptr<u3le_t> _v) { m__dirty = true; m_event_stream_length_in_bytes = std::move(_v); }
        u3le_t* offset_from_bottom_of_sequence_to_sequence_start() const { return m_offset_from_bottom_of_sequence_to_sequence_start.get(); }
        void set_offset_from_bottom_of_sequence_to_sequence_start(std::unique_ptr<u3le_t> _v) { m__dirty = true; m_offset_from_bottom_of_sequence_to_sequence_start = std::move(_v); }
        std::string sequence_name() const { return m_sequence_name; }
        void set_sequence_name(std::string _v) { m__dirty = true; m_sequence_name = std::move(_v); }
        off_on_t loop_to_bar() const { return m_loop_to_bar; }
        void set_loop_to_bar(off_on_t _v) { m__dirty = true; m_loop_to_bar = std::move(_v); }
        uint64_t _unnamed5() const { return m__unnamed5; }
        void set__unnamed5(uint64_t _v) { m__dirty = true; m__unnamed5 = std::move(_v); }
        uint16_t loop_to_bar_number() const { return m_loop_to_bar_number; }
        void set_loop_to_bar_number(uint16_t _v) { m__dirty = true; m_loop_to_bar_number = std::move(_v); }
        uint16_t number_of_bars() const { return m_number_of_bars; }
        void set_number_of_bars(uint16_t _v) { m__dirty = true; m_number_of_bars = std::move(_v); }
        uint32_t length_in_ticks() const { return m_length_in_ticks; }
        void set_length_in_ticks(uint32_t _v) { m__dirty = true; m_length_in_ticks = std::move(_v); }
        uint16_t tempo() const { return m_tempo; }
        void set_tempo(uint16_t _v) { m__dirty = true; m_tempo = std::move(_v); }
        smpte_offset_t* smpte_offset() const { return m_smpte_offset.get(); }
        void set_smpte_offset(std::unique_ptr<smpte_offset_t> _v) { m__dirty = true; m_smpte_offset = std::move(_v); }
        std::vector<uint8_t>* stereo_mix() const { return m_stereo_mix.get(); }
        void set_stereo_mix(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_stereo_mix = std::move(_v); }
        std::vector<uint8_t>* stereo_pan() const { return m_stereo_pan.get(); }
        void set_stereo_pan(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_stereo_pan = std::move(_v); }
        std::vector<uint8_t>* echo_mix() const { return m_echo_mix.get(); }
        void set_echo_mix(std::unique_ptr<std::vector<uint8_t>> _v) { m__dirty = true; m_echo_mix = std::move(_v); }
        std::vector<uint16_t>* drum_tuning() const { return m_drum_tuning.get(); }
        void set_drum_tuning(std::unique_ptr<std::vector<uint16_t>> _v) { m__dirty = true; m_drum_tuning = std::move(_v); }
        uint8_t last_active_user_track() const { return m_last_active_user_track; }
        void set_last_active_user_track(uint8_t _v) { m__dirty = true; m_last_active_user_track = std::move(_v); }
        uint8_t num_tempo_changes() const { return m_num_tempo_changes; }
        void set_num_tempo_changes(uint8_t _v) { m__dirty = true; m_num_tempo_changes = std::move(_v); }
        uint8_t num_track_headers() const { return m_num_track_headers; }
        void set_num_track_headers(uint8_t _v) { m__dirty = true; m_num_track_headers = std::move(_v); }
        mpc60_seq_body_t* _root() const { return m__root; }
        void set__root(mpc60_seq_body_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc60_seq_body_t::sequence_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_sequence_number;
        std::unique_ptr<u3le_t> m_event_stream_length_in_bytes;
        std::unique_ptr<u3le_t> m_offset_from_bottom_of_sequence_to_sequence_start;
        std::string m_sequence_name;
        off_on_t m_loop_to_bar;
        uint64_t m__unnamed5;
        uint16_t m_loop_to_bar_number;
        uint16_t m_number_of_bars;
        uint32_t m_length_in_ticks;
        uint16_t m_tempo;
        std::unique_ptr<smpte_offset_t> m_smpte_offset;
        std::unique_ptr<std::vector<uint8_t>> m_stereo_mix;
        std::unique_ptr<std::vector<uint8_t>> m_stereo_pan;
        std::unique_ptr<std::vector<uint8_t>> m_echo_mix;
        std::unique_ptr<std::vector<uint16_t>> m_drum_tuning;
        uint8_t m_last_active_user_track;
        uint8_t m_num_tempo_changes;
        uint8_t m_num_track_headers;
        mpc60_seq_body_t* m__root;
        mpc60_seq_body_t::sequence_t* m__parent;
    };

    class smpte_offset_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        smpte_offset_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_header_t* p__parent = nullptr, mpc60_seq_body_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~smpte_offset_t();
        uint8_t hundredth_frames() const { return m_hundredth_frames; }
        void set_hundredth_frames(uint8_t _v) { m__dirty = true; m_hundredth_frames = std::move(_v); }
        uint8_t frames() const { return m_frames; }
        void set_frames(uint8_t _v) { m__dirty = true; m_frames = std::move(_v); }
        uint8_t seconds() const { return m_seconds; }
        void set_seconds(uint8_t _v) { m__dirty = true; m_seconds = std::move(_v); }
        uint8_t minutes() const { return m_minutes; }
        void set_minutes(uint8_t _v) { m__dirty = true; m_minutes = std::move(_v); }
        uint8_t hours() const { return m_hours; }
        void set_hours(uint8_t _v) { m__dirty = true; m_hours = std::move(_v); }
        mpc60_seq_body_t* _root() const { return m__root; }
        void set__root(mpc60_seq_body_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc60_seq_body_t::sequence_header_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_hundredth_frames;
        uint8_t m_frames;
        uint8_t m_seconds;
        uint8_t m_minutes;
        uint8_t m_hours;
        mpc60_seq_body_t* m__root;
        mpc60_seq_body_t::sequence_header_t* m__parent;
    };

    class u3le_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        u3le_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_header_t* p__parent = nullptr, mpc60_seq_body_t* p__root = nullptr);
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
        mpc60_seq_body_t* _root() const { return m__root; }
        void set__root(mpc60_seq_body_t* _v) { m__dirty = true; f_value = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc60_seq_body_t::sequence_header_t* _v) { m__dirty = true; f_value = false; m__parent = std::move(_v); }

    private:
        bool f_value;
        int32_t m_value;
        uint16_t m_b12;
        uint8_t m_b3;
        mpc60_seq_body_t* m__root;
        mpc60_seq_body_t::sequence_header_t* m__parent;
    };

public:
    mpc60_seq_body_t* _root() const { return m__root; }
    void set__root(mpc60_seq_body_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    mpc60_seq_body_t* m__root;
    kaitai::kstruct* m__parent;
};

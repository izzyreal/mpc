#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class standard_midi_file_with_running_status_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include "vlq_base128_be.h"
#include <vector>
#include <set>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

/**
 * Standard MIDI file, typically known just as "MID", is a standard way
 * to serialize series of MIDI events, which is a protocol used in many
 * music synthesizers to transfer music data: notes being played,
 * effects being applied, etc.
 * 
 * Internally, file consists of a header and series of tracks, every
 * track listing MIDI events with certain header designating time these
 * events are happening.
 * 
 * NOTE: Rarely, MIDI files employ certain stateful compression scheme
 * to avoid storing certain elements of further elements, instead
 * reusing them from events which happened earlier in the
 * stream. Kaitai Struct (as of v0.9) is currently unable to parse
 * these, but files employing this mechanism are relatively rare.
 */

class standard_midi_file_with_running_status_t : public kaitai::kstruct {

public:
    class channel_pressure_event_t;
    class controller_event_t;
    class header_t;
    class meta_event_body_t;
    class note_off_event_t;
    class note_on_event_t;
    class pitch_bend_event_t;
    class polyphonic_pressure_event_t;
    class program_change_event_t;
    class sysex_event_body_t;
    class track_t;
    class track_event_t;
    class track_events_t;

private:
    bool m__dirty;

public:

    standard_midi_file_with_running_status_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~standard_midi_file_with_running_status_t();

    class channel_pressure_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        channel_pressure_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~channel_pressure_event_t();
        uint8_t pressure() const { return m_pressure; }
        void set_pressure(uint8_t _v) { m__dirty = true; m_pressure = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_pressure;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class controller_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        controller_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~controller_event_t();
        uint8_t controller() const { return m_controller; }
        void set_controller(uint8_t _v) { m__dirty = true; m_controller = std::move(_v); }
        uint8_t value() const { return m_value; }
        void set_value(uint8_t _v) { m__dirty = true; m_value = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_controller;
        uint8_t m_value;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class header_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        header_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~header_t();
        std::string magic() const { return m_magic; }
        void set_magic(std::string _v) { m__dirty = true; m_magic = std::move(_v); }
        uint32_t len_header() const { return m_len_header; }
        void set_len_header(uint32_t _v) { m__dirty = true; m_len_header = std::move(_v); }
        uint16_t format() const { return m_format; }
        void set_format(uint16_t _v) { m__dirty = true; m_format = std::move(_v); }
        uint16_t num_tracks() const { return m_num_tracks; }
        void set_num_tracks(uint16_t _v) { m__dirty = true; m_num_tracks = std::move(_v); }
        int16_t division() const { return m_division; }
        void set_division(int16_t _v) { m__dirty = true; m_division = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m_magic;
        uint32_t m_len_header;
        uint16_t m_format;
        uint16_t m_num_tracks;
        int16_t m_division;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t* m__parent;
    };

    class meta_event_body_t : public kaitai::kstruct {

    public:

        enum meta_type_enum_t {
            META_TYPE_ENUM_SEQUENCE_NUMBER = 0,
            META_TYPE_ENUM_TEXT_EVENT = 1,
            META_TYPE_ENUM_COPYRIGHT = 2,
            META_TYPE_ENUM_SEQUENCE_TRACK_NAME = 3,
            META_TYPE_ENUM_INSTRUMENT_NAME = 4,
            META_TYPE_ENUM_LYRIC_TEXT = 5,
            META_TYPE_ENUM_MARKER_TEXT = 6,
            META_TYPE_ENUM_CUE_POINT = 7,
            META_TYPE_ENUM_MIDI_CHANNEL_PREFIX_ASSIGNMENT = 32,
            META_TYPE_ENUM_END_OF_TRACK = 47,
            META_TYPE_ENUM_TEMPO = 81,
            META_TYPE_ENUM_SMPTE_OFFSET = 84,
            META_TYPE_ENUM_TIME_SIGNATURE = 88,
            META_TYPE_ENUM_KEY_SIGNATURE = 89,
            META_TYPE_ENUM_SEQUENCER_SPECIFIC_EVENT = 127
        };
        static bool _is_defined_meta_type_enum_t(meta_type_enum_t v);

    private:
        static const std::set<meta_type_enum_t> _values_meta_type_enum_t;

    public:

    private:
        bool m__dirty;

    public:

        meta_event_body_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~meta_event_body_t();
        meta_type_enum_t meta_type() const { return m_meta_type; }
        void set_meta_type(meta_type_enum_t _v) { m__dirty = true; m_meta_type = std::move(_v); }
        vlq_base128_be_t* len() const { return m_len.get(); }
        void set_len(std::unique_ptr<vlq_base128_be_t> _v) { m__dirty = true; m_len = std::move(_v); }
        std::string body() const { return m_body; }
        void set_body(std::string _v) { m__dirty = true; m_body = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        meta_type_enum_t m_meta_type;
        std::unique_ptr<vlq_base128_be_t> m_len;
        std::string m_body;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class note_off_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        note_off_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~note_off_event_t();
        uint8_t note() const { return m_note; }
        void set_note(uint8_t _v) { m__dirty = true; m_note = std::move(_v); }
        uint8_t velocity() const { return m_velocity; }
        void set_velocity(uint8_t _v) { m__dirty = true; m_velocity = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_note;
        uint8_t m_velocity;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class note_on_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        note_on_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~note_on_event_t();
        uint8_t note() const { return m_note; }
        void set_note(uint8_t _v) { m__dirty = true; m_note = std::move(_v); }
        uint8_t velocity() const { return m_velocity; }
        void set_velocity(uint8_t _v) { m__dirty = true; m_velocity = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_note;
        uint8_t m_velocity;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class pitch_bend_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        pitch_bend_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~pitch_bend_event_t();
        int32_t adj_bend_value();
        void _invalidate_adj_bend_value() { f_adj_bend_value = false; }
        int32_t bend_value();
        void _invalidate_bend_value() { f_bend_value = false; }
        uint8_t b1() const { return m_b1; }
        void set_b1(uint8_t _v) { m__dirty = true; f_adj_bend_value = false; f_bend_value = false; m_b1 = std::move(_v); }
        uint8_t b2() const { return m_b2; }
        void set_b2(uint8_t _v) { m__dirty = true; f_adj_bend_value = false; f_bend_value = false; m_b2 = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; f_adj_bend_value = false; f_bend_value = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; f_adj_bend_value = false; f_bend_value = false; m__parent = std::move(_v); }

    private:
        bool f_adj_bend_value;
        int32_t m_adj_bend_value;
        bool f_bend_value;
        int32_t m_bend_value;
        uint8_t m_b1;
        uint8_t m_b2;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class polyphonic_pressure_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        polyphonic_pressure_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~polyphonic_pressure_event_t();
        uint8_t note() const { return m_note; }
        void set_note(uint8_t _v) { m__dirty = true; m_note = std::move(_v); }
        uint8_t pressure() const { return m_pressure; }
        void set_pressure(uint8_t _v) { m__dirty = true; m_pressure = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_note;
        uint8_t m_pressure;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class program_change_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        program_change_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~program_change_event_t();
        uint8_t program() const { return m_program; }
        void set_program(uint8_t _v) { m__dirty = true; m_program = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_program;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class sysex_event_body_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sysex_event_body_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sysex_event_body_t();
        vlq_base128_be_t* len() const { return m_len.get(); }
        void set_len(std::unique_ptr<vlq_base128_be_t> _v) { m__dirty = true; m_len = std::move(_v); }
        std::string data() const { return m_data; }
        void set_data(std::string _v) { m__dirty = true; m_data = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_event_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::unique_ptr<vlq_base128_be_t> m_len;
        std::string m_data;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_event_t* m__parent;
    };

    class track_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        track_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~track_t();
        std::string magic() const { return m_magic; }
        void set_magic(std::string _v) { m__dirty = true; m_magic = std::move(_v); }
        uint32_t len_events() const { return m_len_events; }
        void set_len_events(uint32_t _v) { m__dirty = true; m_len_events = std::move(_v); }
        track_events_t* events() const { return m_events.get(); }
        void set_events(std::unique_ptr<track_events_t> _v) { m__dirty = true; m_events = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__parent = std::move(_v); }
        std::string _raw_events() const { return m__raw_events; }
        void set__raw_events(std::string _v) { m__dirty = true; m__raw_events = std::move(_v); }
        kaitai::kstream* _io__raw_events() const { return m__io__raw_events.get(); }
        void set__io__raw_events(std::unique_ptr<kaitai::kstream> _v) { m__io__raw_events = std::move(_v); }

    private:
        std::string m_magic;
        uint32_t m_len_events;
        std::unique_ptr<track_events_t> m_events;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t* m__parent;
        std::string m__raw_events;
        std::unique_ptr<kaitai::kstream> m__io__raw_events;
    };

    class track_event_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        track_event_t(uint8_t p_previous_status, kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_events_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~track_event_t();
        int32_t channel();
        void _invalidate_channel() { f_channel = false; }
        uint8_t effective_status();
        void _invalidate_effective_status() { f_effective_status = false; }
        int32_t event_type();
        void _invalidate_event_type() { f_event_type = false; }
        uint8_t status_byte_lookahead();
        void set_status_byte_lookahead(uint8_t _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; e_status_byte_lookahead = false; f_status_byte_lookahead = true; m_status_byte_lookahead = std::move(_v); }
        void set_status_byte_lookahead_enabled(bool _v) { m__dirty = true; e_status_byte_lookahead = _v; }

    private:
        void _write_status_byte_lookahead();

    public:
        bool using_running_status();
        void _invalidate_using_running_status() { f_using_running_status = false; }
        vlq_base128_be_t* v_time() const { return m_v_time.get(); }
        void set_v_time(std::unique_ptr<vlq_base128_be_t> _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m_v_time = std::move(_v); }
        uint8_t event_header() const { return m_event_header; }
        void set_event_header(uint8_t _v) { m__dirty = true; n_event_header = false; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m_event_header = std::move(_v); }
        meta_event_body_t* meta_event_body() const { return m_meta_event_body.get(); }
        void set_meta_event_body(std::unique_ptr<meta_event_body_t> _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m_meta_event_body = std::move(_v); }
        sysex_event_body_t* sysex_body() const { return m_sysex_body.get(); }
        void set_sysex_body(std::unique_ptr<sysex_event_body_t> _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m_sysex_body = std::move(_v); }
        kaitai::kstruct* event_body() const { return m_event_body.get(); }
        void set_event_body(std::unique_ptr<kaitai::kstruct> _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m_event_body = std::move(_v); }
        uint8_t previous_status() const { return m_previous_status; }
        void set_previous_status(uint8_t _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m_previous_status = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_events_t* _v) { m__dirty = true; f_channel = false; f_effective_status = false; f_event_type = false; f_using_running_status = false; m__parent = std::move(_v); }

    private:
        bool f_channel;
        int32_t m_channel;
        bool n_channel;

    public:
        bool _is_null_channel() { channel(); return n_channel; };

    private:
        bool f_effective_status;
        uint8_t m_effective_status;
        bool f_event_type;
        int32_t m_event_type;
        bool f_status_byte_lookahead;
        uint8_t m_status_byte_lookahead;
        bool w_status_byte_lookahead;
        bool e_status_byte_lookahead;
        bool f_using_running_status;
        bool m_using_running_status;
        std::unique_ptr<vlq_base128_be_t> m_v_time;
        uint8_t m_event_header;
        bool n_event_header;

    public:
        bool _is_null_event_header() { event_header(); return n_event_header; };

    private:
        std::unique_ptr<meta_event_body_t> m_meta_event_body;

    public:
        bool _is_null_meta_event_body() { return !meta_event_body(); };

    private:
        std::unique_ptr<sysex_event_body_t> m_sysex_body;

    public:
        bool _is_null_sysex_body() { return !sysex_body(); };

    private:
        std::unique_ptr<kaitai::kstruct> m_event_body;

    public:
        bool _is_null_event_body() { return !event_body(); };

    private:
        uint8_t m_previous_status;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_events_t* m__parent;
    };

    class track_events_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        track_events_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_t* p__parent = nullptr, standard_midi_file_with_running_status_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~track_events_t();
        std::vector<std::unique_ptr<track_event_t>>* event() const { return m_event.get(); }
        void set_event(std::unique_ptr<std::vector<std::unique_ptr<track_event_t>>> _v) { m__dirty = true; m_event = std::move(_v); }
        standard_midi_file_with_running_status_t* _root() const { return m__root; }
        void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(standard_midi_file_with_running_status_t::track_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::unique_ptr<std::vector<std::unique_ptr<track_event_t>>> m_event;
        standard_midi_file_with_running_status_t* m__root;
        standard_midi_file_with_running_status_t::track_t* m__parent;
    };

public:
    header_t* hdr() const { return m_hdr.get(); }
    void set_hdr(std::unique_ptr<header_t> _v) { m__dirty = true; m_hdr = std::move(_v); }
    std::vector<std::unique_ptr<track_t>>* tracks() const { return m_tracks.get(); }
    void set_tracks(std::unique_ptr<std::vector<std::unique_ptr<track_t>>> _v) { m__dirty = true; m_tracks = std::move(_v); }
    standard_midi_file_with_running_status_t* _root() const { return m__root; }
    void set__root(standard_midi_file_with_running_status_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::unique_ptr<header_t> m_hdr;
    std::unique_ptr<std::vector<std::unique_ptr<track_t>>> m_tracks;
    standard_midi_file_with_running_status_t* m__root;
    kaitai::kstruct* m__parent;
};

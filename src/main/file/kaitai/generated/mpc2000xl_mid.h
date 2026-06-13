#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc2000xl_mid_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include "standard_midi_file_with_running_status.h"
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc2000xl_mid_t : public kaitai::kstruct {

public:
    class sequencer_t;

private:
    bool m__dirty;

public:

    mpc2000xl_mid_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc2000xl_mid_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc2000xl_mid_t();

    class sequencer_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sequencer_t(kaitai::kstream* p__io, mpc2000xl_mid_t* p__parent = nullptr, mpc2000xl_mid_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequencer_t();
        std::string _unnamed0() const { return m__unnamed0; }
        void set__unnamed0(std::string _v) { m__dirty = true; m__unnamed0 = std::move(_v); }
        std::string loop_enabled() const { return m_loop_enabled; }
        void set_loop_enabled(std::string _v) { m__dirty = true; m_loop_enabled = std::move(_v); }
        std::string _unnamed2() const { return m__unnamed2; }
        void set__unnamed2(std::string _v) { m__dirty = true; m__unnamed2 = std::move(_v); }
        std::string loop_start() const { return m_loop_start; }
        void set_loop_start(std::string _v) { m__dirty = true; m_loop_start = std::move(_v); }
        std::string _unnamed4() const { return m__unnamed4; }
        void set__unnamed4(std::string _v) { m__dirty = true; m__unnamed4 = std::move(_v); }
        std::string loop_end() const { return m_loop_end; }
        void set_loop_end(std::string _v) { m__dirty = true; m_loop_end = std::move(_v); }
        std::string _unnamed6() const { return m__unnamed6; }
        void set__unnamed6(std::string _v) { m__dirty = true; m__unnamed6 = std::move(_v); }
        std::string tempo_source() const { return m_tempo_source; }
        void set_tempo_source(std::string _v) { m__dirty = true; m_tempo_source = std::move(_v); }
        mpc2000xl_mid_t* _root() const { return m__root; }
        void set__root(mpc2000xl_mid_t* _v) { m__dirty = true; m__root = std::move(_v); }
        mpc2000xl_mid_t* _parent() const { return m__parent; }
        void set__parent(mpc2000xl_mid_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m__unnamed0;
        std::string m_loop_enabled;
        std::string m__unnamed2;
        std::string m_loop_start;
        std::string m__unnamed4;
        std::string m_loop_end;
        std::string m__unnamed6;
        std::string m_tempo_source;
        mpc2000xl_mid_t* m__root;
        mpc2000xl_mid_t* m__parent;
    };

public:
    int32_t denominator();
    void _invalidate_denominator() { f_denominator = false; }
    standard_midi_file_with_running_status_t::meta_event_body_t* first_meta();
    void _invalidate_first_meta() { f_first_meta = false; }
    bool is_mpc2000xl_mid();
    void _invalidate_is_mpc2000xl_mid() { f_is_mpc2000xl_mid = false; }
    std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_event_t>>* meta_events();
    void _invalidate_meta_events() { f_meta_events = false; }
    uint8_t numerator();
    void _invalidate_numerator() { f_numerator = false; }
    std::string sequence_name();
    void _invalidate_sequence_name() { f_sequence_name = false; }
    mpc2000xl_mid_t::sequencer_t* sequencer();
    void set_sequencer(std::unique_ptr<sequencer_t> _v) { m__dirty = true; f_denominator = false; f_first_meta = false; f_is_mpc2000xl_mid = false; f_meta_events = false; f_numerator = false; f_sequence_name = false; f_smpte_offset = false; f_tempo_bpm = false; f_sequencer = true; m_sequencer = std::move(_v); }
    void set_sequencer_enabled(bool _v) { m__dirty = true; e_sequencer = _v; }

private:
    void _write_sequencer();

public:
    std::string smpte_offset();
    void _invalidate_smpte_offset() { f_smpte_offset = false; }
    double tempo_bpm();
    void _invalidate_tempo_bpm() { f_tempo_bpm = false; }
    standard_midi_file_with_running_status_t::header_t* hdr() const { return m_hdr.get(); }
    void set_hdr(std::unique_ptr<standard_midi_file_with_running_status_t::header_t> _v) { m__dirty = true; f_denominator = false; f_first_meta = false; f_is_mpc2000xl_mid = false; f_meta_events = false; f_numerator = false; f_sequence_name = false; f_smpte_offset = false; f_tempo_bpm = false; m_hdr = std::move(_v); }
    std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_t>>* tracks() const { return m_tracks.get(); }
    void set_tracks(std::unique_ptr<std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_t>>> _v) { m__dirty = true; f_denominator = false; f_first_meta = false; f_is_mpc2000xl_mid = false; f_meta_events = false; f_numerator = false; f_sequence_name = false; f_smpte_offset = false; f_tempo_bpm = false; m_tracks = std::move(_v); }
    mpc2000xl_mid_t* _root() const { return m__root; }
    void set__root(mpc2000xl_mid_t* _v) { m__dirty = true; f_denominator = false; f_first_meta = false; f_is_mpc2000xl_mid = false; f_meta_events = false; f_numerator = false; f_sequence_name = false; f_smpte_offset = false; f_tempo_bpm = false; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; f_denominator = false; f_first_meta = false; f_is_mpc2000xl_mid = false; f_meta_events = false; f_numerator = false; f_sequence_name = false; f_smpte_offset = false; f_tempo_bpm = false; m__parent = std::move(_v); }

private:
    bool f_denominator;
    int32_t m_denominator;
    bool f_first_meta;
    standard_midi_file_with_running_status_t::meta_event_body_t* m_first_meta;
    bool f_is_mpc2000xl_mid;
    bool m_is_mpc2000xl_mid;
    bool f_meta_events;
    std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_event_t>>* m_meta_events;
    bool f_numerator;
    uint8_t m_numerator;
    bool f_sequence_name;
    std::string m_sequence_name;
    bool f_sequencer;
    std::unique_ptr<mpc2000xl_mid_t::sequencer_t> m_sequencer;
    bool w_sequencer;
    bool e_sequencer;
    bool f_smpte_offset;
    std::string m_smpte_offset;
    bool f_tempo_bpm;
    double m_tempo_bpm;
    std::unique_ptr<standard_midi_file_with_running_status_t::header_t> m_hdr;
    std::unique_ptr<std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_t>>> m_tracks;
    mpc2000xl_mid_t* m__root;
    kaitai::kstruct* m__parent;
};

#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc3000_all_v3_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include "mpc3000_seq_v3.h"
#include <vector>
#include <set>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc3000_all_v3_t : public kaitai::kstruct {

public:
    class all_file_header_t;
    class misc_chunks_t;
    class sequence_t;
    class song_t;

private:
    bool m__dirty;

public:

    mpc3000_all_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc3000_all_v3_t();

    class all_file_header_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        all_file_header_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~all_file_header_t();
        std::string file_id() const { return m_file_id; }
        void set_file_id(std::string _v) { m__dirty = true; m_file_id = std::move(_v); }
        std::string file_version() const { return m_file_version; }
        void set_file_version(std::string _v) { m__dirty = true; m_file_version = std::move(_v); }
        uint32_t total_number_of_bytes_in_all_sequences() const { return m_total_number_of_bytes_in_all_sequences; }
        void set_total_number_of_bytes_in_all_sequences(uint32_t _v) { m__dirty = true; m_total_number_of_bytes_in_all_sequences = std::move(_v); }
        mpc3000_all_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_all_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc3000_all_v3_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::string m_file_id;
        std::string m_file_version;
        uint32_t m_total_number_of_bytes_in_all_sequences;
        mpc3000_all_v3_t* m__root;
        mpc3000_all_v3_t* m__parent;
    };

    class misc_chunks_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        misc_chunks_t(kaitai::kstream* p__io, mpc3000_all_v3_t::sequence_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~misc_chunks_t();
        int32_t position_after_tempo_changes();
        void _invalidate_position_after_tempo_changes() { f_position_after_tempo_changes = false; }
        mpc3000_seq_v3_t::sequence_header_t* sequence_header() const { return m_sequence_header.get(); }
        void set_sequence_header(std::unique_ptr<mpc3000_seq_v3_t::sequence_header_t> _v) { m__dirty = true; f_position_after_tempo_changes = false; m_sequence_header = std::move(_v); }
        mpc3000_seq_v3_t::smpte_offset_t* smpte_offset() const { return m_smpte_offset.get(); }
        void set_smpte_offset(std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t> _v) { m__dirty = true; f_position_after_tempo_changes = false; m_smpte_offset = std::move(_v); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>* mixer() const { return m_mixer.get(); }
        void set_mixer(std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>> _v) { m__dirty = true; f_position_after_tempo_changes = false; m_mixer = std::move(_v); }
        std::string reserved_after_mixer() const { return m_reserved_after_mixer; }
        void set_reserved_after_mixer(std::string _v) { m__dirty = true; f_position_after_tempo_changes = false; m_reserved_after_mixer = std::move(_v); }
        mpc3000_seq_v3_t::delays_t* delays() const { return m_delays.get(); }
        void set_delays(std::unique_ptr<mpc3000_seq_v3_t::delays_t> _v) { m__dirty = true; f_position_after_tempo_changes = false; m_delays = std::move(_v); }
        std::string reserved_after_delays() const { return m_reserved_after_delays; }
        void set_reserved_after_delays(std::string _v) { m__dirty = true; f_position_after_tempo_changes = false; m_reserved_after_delays = std::move(_v); }
        std::string reserved_before_track_summary() const { return m_reserved_before_track_summary; }
        void set_reserved_before_track_summary(std::string _v) { m__dirty = true; f_position_after_tempo_changes = false; m_reserved_before_track_summary = std::move(_v); }
        uint8_t last_active_track() const { return m_last_active_track; }
        void set_last_active_track(uint8_t _v) { m__dirty = true; f_position_after_tempo_changes = false; m_last_active_track = std::move(_v); }
        uint8_t num_tempo_changes() const { return m_num_tempo_changes; }
        void set_num_tempo_changes(uint8_t _v) { m__dirty = true; f_position_after_tempo_changes = false; m_num_tempo_changes = std::move(_v); }
        uint8_t num_track_headers() const { return m_num_track_headers; }
        void set_num_track_headers(uint8_t _v) { m__dirty = true; f_position_after_tempo_changes = false; m_num_track_headers = std::move(_v); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>* track_headers() const { return m_track_headers.get(); }
        void set_track_headers(std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>> _v) { m__dirty = true; f_position_after_tempo_changes = false; m_track_headers = std::move(_v); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>* tempo_changes() const { return m_tempo_changes.get(); }
        void set_tempo_changes(std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>> _v) { m__dirty = true; f_position_after_tempo_changes = false; m_tempo_changes = std::move(_v); }
        mpc3000_all_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_all_v3_t* _v) { m__dirty = true; f_position_after_tempo_changes = false; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc3000_all_v3_t::sequence_t* _v) { m__dirty = true; f_position_after_tempo_changes = false; m__parent = std::move(_v); }

    private:
        bool f_position_after_tempo_changes;
        int32_t m_position_after_tempo_changes;
        std::unique_ptr<mpc3000_seq_v3_t::sequence_header_t> m_sequence_header;
        std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t> m_smpte_offset;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>> m_mixer;
        std::string m_reserved_after_mixer;
        std::unique_ptr<mpc3000_seq_v3_t::delays_t> m_delays;
        std::string m_reserved_after_delays;
        std::string m_reserved_before_track_summary;
        uint8_t m_last_active_track;
        uint8_t m_num_tempo_changes;
        uint8_t m_num_track_headers;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>> m_track_headers;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>> m_tempo_changes;
        mpc3000_all_v3_t* m__root;
        mpc3000_all_v3_t::sequence_t* m__parent;
    };

    class sequence_t : public kaitai::kstruct {

    public:

    private:
        bool m__dirty;

    public:

        sequence_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~sequence_t();
        misc_chunks_t* misc_chunks() const { return m_misc_chunks.get(); }
        void set_misc_chunks(std::unique_ptr<misc_chunks_t> _v) { m__dirty = true; m_misc_chunks = std::move(_v); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>* events() const { return m_events.get(); }
        void set_events(std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>> _v) { m__dirty = true; m_events = std::move(_v); }
        mpc3000_all_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_all_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc3000_all_v3_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        std::unique_ptr<misc_chunks_t> m_misc_chunks;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>> m_events;
        mpc3000_all_v3_t* m__root;
        mpc3000_all_v3_t* m__parent;
    };

    class song_t : public kaitai::kstruct {

    public:
        class song_body_t;

    private:
        bool m__dirty;

    public:

        song_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);
        void _read();
        void _fetch_instances();
        void _write();
        void _check();
        ~song_t();

        class song_body_t : public kaitai::kstruct {

        public:
            class step_t;

            enum end_status_t {
                END_STATUS_STOP_AT_END = 0,
                END_STATUS_LOOP_TO_A_STEP = 1
            };
            static bool _is_defined_end_status_t(end_status_t v);

        private:
            static const std::set<end_status_t> _values_end_status_t;

        public:

        private:
            bool m__dirty;

        public:

            song_body_t(kaitai::kstream* p__io, mpc3000_all_v3_t::song_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);
            void _read();
            void _fetch_instances();
            void _write();
            void _check();
            ~song_body_t();

            class step_t : public kaitai::kstruct {

            public:

            private:
                bool m__dirty;

            public:

                step_t(kaitai::kstream* p__io, mpc3000_all_v3_t::song_t::song_body_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);
                void _read();
                void _fetch_instances();
                void _write();
                void _check();
                ~step_t();
                uint8_t sequence_number() const { return m_sequence_number; }
                void set_sequence_number(uint8_t _v) { m__dirty = true; m_sequence_number = std::move(_v); }
                uint8_t repetition_count() const { return m_repetition_count; }
                void set_repetition_count(uint8_t _v) { m__dirty = true; m_repetition_count = std::move(_v); }
                mpc3000_all_v3_t* _root() const { return m__root; }
                void set__root(mpc3000_all_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
                kaitai::kstruct* _parent() const { return m__parent; }
                void set__parent(mpc3000_all_v3_t::song_t::song_body_t* _v) { m__dirty = true; m__parent = std::move(_v); }

            private:
                uint8_t m_sequence_number;
                uint8_t m_repetition_count;
                mpc3000_all_v3_t* m__root;
                mpc3000_all_v3_t::song_t::song_body_t* m__parent;
            };

        public:
            uint8_t song_number() const { return m_song_number; }
            void set_song_number(uint8_t _v) { m__dirty = true; m_song_number = std::move(_v); }
            end_status_t end_status() const { return m_end_status; }
            void set_end_status(end_status_t _v) { m__dirty = true; m_end_status = std::move(_v); }
            uint8_t loop_back_step_number() const { return m_loop_back_step_number; }
            void set_loop_back_step_number(uint8_t _v) { m__dirty = true; m_loop_back_step_number = std::move(_v); }
            std::string song_name() const { return m_song_name; }
            void set_song_name(std::string _v) { m__dirty = true; m_song_name = std::move(_v); }
            mpc3000_seq_v3_t::smpte_offset_t* smpte_offset() const { return m_smpte_offset.get(); }
            void set_smpte_offset(std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t> _v) { m__dirty = true; m_smpte_offset = std::move(_v); }
            std::vector<std::unique_ptr<step_t>>* steps() const { return m_steps.get(); }
            void set_steps(std::unique_ptr<std::vector<std::unique_ptr<step_t>>> _v) { m__dirty = true; m_steps = std::move(_v); }
            mpc3000_all_v3_t* _root() const { return m__root; }
            void set__root(mpc3000_all_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
            kaitai::kstruct* _parent() const { return m__parent; }
            void set__parent(mpc3000_all_v3_t::song_t* _v) { m__dirty = true; m__parent = std::move(_v); }

        private:
            uint8_t m_song_number;
            end_status_t m_end_status;
            uint8_t m_loop_back_step_number;
            std::string m_song_name;
            std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t> m_smpte_offset;
            std::unique_ptr<std::vector<std::unique_ptr<step_t>>> m_steps;
            mpc3000_all_v3_t* m__root;
            mpc3000_all_v3_t::song_t* m__parent;
        };

    public:
        uint8_t number_of_steps() const { return m_number_of_steps; }
        void set_number_of_steps(uint8_t _v) { m__dirty = true; m_number_of_steps = std::move(_v); }
        song_body_t* song_body() const { return m_song_body.get(); }
        void set_song_body(std::unique_ptr<song_body_t> _v) { m__dirty = true; m_song_body = std::move(_v); }
        mpc3000_all_v3_t* _root() const { return m__root; }
        void set__root(mpc3000_all_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
        kaitai::kstruct* _parent() const { return m__parent; }
        void set__parent(mpc3000_all_v3_t* _v) { m__dirty = true; m__parent = std::move(_v); }

    private:
        uint8_t m_number_of_steps;
        std::unique_ptr<song_body_t> m_song_body;

    public:
        bool _is_null_song_body() { return !song_body(); };

    private:
        mpc3000_all_v3_t* m__root;
        mpc3000_all_v3_t* m__parent;
    };

public:
    all_file_header_t* all_file_header() const { return m_all_file_header.get(); }
    void set_all_file_header(std::unique_ptr<all_file_header_t> _v) { m__dirty = true; m_all_file_header = std::move(_v); }
    std::vector<std::unique_ptr<sequence_t>>* sequences() const { return m_sequences.get(); }
    void set_sequences(std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>> _v) { m__dirty = true; m_sequences = std::move(_v); }
    std::string sequences_terminator() const { return m_sequences_terminator; }
    void set_sequences_terminator(std::string _v) { m__dirty = true; m_sequences_terminator = std::move(_v); }
    std::vector<std::unique_ptr<song_t>>* songs() const { return m_songs.get(); }
    void set_songs(std::unique_ptr<std::vector<std::unique_ptr<song_t>>> _v) { m__dirty = true; m_songs = std::move(_v); }
    mpc3000_all_v3_t* _root() const { return m__root; }
    void set__root(mpc3000_all_v3_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::unique_ptr<all_file_header_t> m_all_file_header;
    std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>> m_sequences;
    std::string m_sequences_terminator;
    std::unique_ptr<std::vector<std::unique_ptr<song_t>>> m_songs;
    mpc3000_all_v3_t* m__root;
    kaitai::kstruct* m__parent;
};

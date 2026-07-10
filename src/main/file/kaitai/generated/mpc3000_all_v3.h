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

    mpc3000_all_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);

private:
    void _read();

public:
    void _fetch_instances();
    ~mpc3000_all_v3_t();

    class all_file_header_t : public kaitai::kstruct {

    public:

        all_file_header_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~all_file_header_t();
        std::string file_id() const { return m_file_id; }
        std::string file_version() const { return m_file_version; }
        uint32_t total_number_of_bytes_in_all_sequences() const { return m_total_number_of_bytes_in_all_sequences; }
        mpc3000_all_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        std::string m_file_id;
        std::string m_file_version;
        uint32_t m_total_number_of_bytes_in_all_sequences;
        mpc3000_all_v3_t* m__root;
        mpc3000_all_v3_t* m__parent;
    };

    class misc_chunks_t : public kaitai::kstruct {

    public:

        misc_chunks_t(kaitai::kstream* p__io, mpc3000_all_v3_t::sequence_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~misc_chunks_t();
        int32_t position_after_tempo_changes();
        mpc3000_seq_v3_t::sequence_header_t* sequence_header() const { return m_sequence_header.get(); }
        mpc3000_seq_v3_t::smpte_offset_t* smpte_offset() const { return m_smpte_offset.get(); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>* mixer() const { return m_mixer.get(); }
        std::string _unnamed3() const { return m__unnamed3; }
        mpc3000_seq_v3_t::delays_t* delays() const { return m_delays.get(); }
        std::string _unnamed5() const { return m__unnamed5; }
        std::string _unnamed6() const { return m__unnamed6; }
        uint8_t last_active_track() const { return m_last_active_track; }
        uint8_t number_of_tempo_changes() const { return m_number_of_tempo_changes; }
        uint8_t number_of_active_track_headers() const { return m_number_of_active_track_headers; }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>* track_headers() const { return m_track_headers.get(); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>* tempo_changes() const { return m_tempo_changes.get(); }
        mpc3000_all_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        bool f_position_after_tempo_changes;
        int32_t m_position_after_tempo_changes;
        std::unique_ptr<mpc3000_seq_v3_t::sequence_header_t> m_sequence_header;
        std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t> m_smpte_offset;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>> m_mixer;
        std::string m__unnamed3;
        std::unique_ptr<mpc3000_seq_v3_t::delays_t> m_delays;
        std::string m__unnamed5;
        std::string m__unnamed6;
        uint8_t m_last_active_track;
        uint8_t m_number_of_tempo_changes;
        uint8_t m_number_of_active_track_headers;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>> m_track_headers;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>> m_tempo_changes;
        mpc3000_all_v3_t* m__root;
        mpc3000_all_v3_t::sequence_t* m__parent;
    };

    class sequence_t : public kaitai::kstruct {

    public:

        sequence_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~sequence_t();
        misc_chunks_t* misc_chunks() const { return m_misc_chunks.get(); }
        std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>* events() const { return m_events.get(); }
        mpc3000_all_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        std::unique_ptr<misc_chunks_t> m_misc_chunks;
        std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>> m_events;
        mpc3000_all_v3_t* m__root;
        mpc3000_all_v3_t* m__parent;
    };

    class song_t : public kaitai::kstruct {

    public:
        class song_body_t;

        song_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
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

            song_body_t(kaitai::kstream* p__io, mpc3000_all_v3_t::song_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);

        private:
            void _read();

        public:
            void _fetch_instances();
            ~song_body_t();

            class step_t : public kaitai::kstruct {

            public:

                step_t(kaitai::kstream* p__io, mpc3000_all_v3_t::song_t::song_body_t* p__parent = nullptr, mpc3000_all_v3_t* p__root = nullptr);

            private:
                void _read();

            public:
                void _fetch_instances();
                ~step_t();
                uint8_t sequence_number() const { return m_sequence_number; }
                uint8_t repetition_count() const { return m_repetition_count; }
                mpc3000_all_v3_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }

            private:
                uint8_t m_sequence_number;
                uint8_t m_repetition_count;
                mpc3000_all_v3_t* m__root;
                mpc3000_all_v3_t::song_t::song_body_t* m__parent;
            };

        public:
            uint8_t song_number() const { return m_song_number; }
            end_status_t end_status() const { return m_end_status; }
            uint8_t loop_back_step_number() const { return m_loop_back_step_number; }
            std::string song_name() const { return m_song_name; }
            mpc3000_seq_v3_t::smpte_offset_t* smpte_offset() const { return m_smpte_offset.get(); }
            std::vector<std::unique_ptr<step_t>>* steps() const { return m_steps.get(); }
            mpc3000_all_v3_t* _root() const { return m__root; }
            kaitai::kstruct* _parent() const { return m__parent; }

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
        song_body_t* song_body() const { return m_song_body.get(); }
        mpc3000_all_v3_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

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
    std::vector<std::unique_ptr<sequence_t>>* sequences() const { return m_sequences.get(); }
    std::vector<std::unique_ptr<song_t>>* songs() const { return m_songs.get(); }
    mpc3000_all_v3_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }

private:
    std::unique_ptr<all_file_header_t> m_all_file_header;
    std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>> m_sequences;
    std::unique_ptr<std::vector<std::unique_ptr<song_t>>> m_songs;
    mpc3000_all_v3_t* m__root;
    kaitai::kstruct* m__parent;
};

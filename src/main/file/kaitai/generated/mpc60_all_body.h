#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc60_all_body_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include "mpc60_seq_body.h"
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc60_all_body_t : public kaitai::kstruct {

public:
    class all_file_body_t;
    class empty_song_t;
    class song_t;
    class song_body_t;
    class song_step_t;

    mpc60_all_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc60_all_body_t* p__root = nullptr);

private:
    void _read();

public:
    void _fetch_instances();
    ~mpc60_all_body_t();

    class all_file_body_t : public kaitai::kstruct {

    public:

        all_file_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc60_all_body_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~all_file_body_t();
        uint32_t total_number_of_bytes_in_all_sequences() const { return m_total_number_of_bytes_in_all_sequences; }
        std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>* sequences() const { return m_sequences.get(); }
        std::string sequences_terminator() const { return m_sequences_terminator; }
        std::vector<std::unique_ptr<song_t>>* songs() const { return m_songs.get(); }
        mpc60_all_body_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint32_t m_total_number_of_bytes_in_all_sequences;
        std::unique_ptr<std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>> m_sequences;
        std::string m_sequences_terminator;
        std::unique_ptr<std::vector<std::unique_ptr<song_t>>> m_songs;
        mpc60_all_body_t* m__root;
        kaitai::kstruct* m__parent;
    };

    class empty_song_t : public kaitai::kstruct {

    public:

        empty_song_t(kaitai::kstream* p__io, mpc60_all_body_t::song_t* p__parent = nullptr, mpc60_all_body_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~empty_song_t();
        mpc60_all_body_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        mpc60_all_body_t* m__root;
        mpc60_all_body_t::song_t* m__parent;
    };

    class song_t : public kaitai::kstruct {

    public:

        song_t(kaitai::kstream* p__io, mpc60_all_body_t::all_file_body_t* p__parent = nullptr, mpc60_all_body_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~song_t();
        uint8_t step_count() const { return m_step_count; }
        kaitai::kstruct* body() const { return m_body.get(); }
        mpc60_all_body_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_step_count;
        std::unique_ptr<kaitai::kstruct> m_body;
        mpc60_all_body_t* m__root;
        mpc60_all_body_t::all_file_body_t* m__parent;
    };

    class song_body_t : public kaitai::kstruct {

    public:

        song_body_t(kaitai::kstream* p__io, mpc60_all_body_t::song_t* p__parent = nullptr, mpc60_all_body_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~song_body_t();
        uint8_t song_number() const { return m_song_number; }
        std::string reserved_1() const { return m_reserved_1; }
        std::string reserved_2() const { return m_reserved_2; }
        std::string song_name() const { return m_song_name; }
        std::string reserved_3() const { return m_reserved_3; }
        std::vector<std::unique_ptr<song_step_t>>* steps() const { return m_steps.get(); }
        mpc60_all_body_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_song_number;
        std::string m_reserved_1;
        std::string m_reserved_2;
        std::string m_song_name;
        std::string m_reserved_3;
        std::unique_ptr<std::vector<std::unique_ptr<song_step_t>>> m_steps;
        mpc60_all_body_t* m__root;
        mpc60_all_body_t::song_t* m__parent;
    };

    class song_step_t : public kaitai::kstruct {

    public:

        song_step_t(kaitai::kstream* p__io, mpc60_all_body_t::song_body_t* p__parent = nullptr, mpc60_all_body_t* p__root = nullptr);

    private:
        void _read();

    public:
        void _fetch_instances();
        ~song_step_t();
        uint8_t sequence_number() const { return m_sequence_number; }
        uint8_t repeats() const { return m_repeats; }
        mpc60_all_body_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }

    private:
        uint8_t m_sequence_number;
        uint8_t m_repeats;
        mpc60_all_body_t* m__root;
        mpc60_all_body_t::song_body_t* m__parent;
    };

public:
    mpc60_all_body_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }

private:
    mpc60_all_body_t* m__root;
    kaitai::kstruct* m__parent;
};

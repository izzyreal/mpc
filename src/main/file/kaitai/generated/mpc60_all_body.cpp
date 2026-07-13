// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_all_body.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>

mpc60_all_body_t::mpc60_all_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    _read();
}

void mpc60_all_body_t::_read() {
}

void mpc60_all_body_t::_fetch_instances() {
}

mpc60_all_body_t::~mpc60_all_body_t() {}

mpc60_all_body_t::all_file_body_t::all_file_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc60_all_body_t::all_file_body_t::_read() {
    m_total_number_of_bytes_in_all_sequences = m__io->read_u4le();
    m_sequences = std::unique_ptr<std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>>(new std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>());
    {
        int i = 0;
        mpc60_seq_body_t::sequence_t* _;
        do {
            _ = new mpc60_seq_body_t::sequence_t(m__io);
            m_sequences->push_back(std::move(std::unique_ptr<mpc60_seq_body_t::sequence_t>(_)));
            i++;
        } while (!(_io()->pos() - 6 >= total_number_of_bytes_in_all_sequences() - 1));
    }
    m_sequences_terminator = m__io->read_bytes(1);
    if (!(m_sequences_terminator == std::string("\xFF", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\xFF", 1), m_sequences_terminator, m__io, std::string("/types/all_file_body/seq/2"));
    }
    m_songs = std::unique_ptr<std::vector<std::unique_ptr<song_t>>>(new std::vector<std::unique_ptr<song_t>>());
    {
        int i = 0;
        song_t* _;
        do {
            _ = new song_t(m__io, this, m__root);
            m_songs->push_back(std::move(std::unique_ptr<song_t>(_)));
            i++;
        } while (!(_->step_count() == 0));
    }
}

void mpc60_all_body_t::all_file_body_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_sequences->size(); ++i) {
        m_sequences->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_songs->size(); ++i) {
        m_songs->at(i).get()->_fetch_instances();
    }
}

mpc60_all_body_t::all_file_body_t::~all_file_body_t() {}

mpc60_all_body_t::empty_song_t::empty_song_t(kaitai::kstream* p__io, mpc60_all_body_t::song_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc60_all_body_t::empty_song_t::_read() {
}

void mpc60_all_body_t::empty_song_t::_fetch_instances() {
}

mpc60_all_body_t::empty_song_t::~empty_song_t() {}

mpc60_all_body_t::song_t::song_t(kaitai::kstream* p__io, mpc60_all_body_t::all_file_body_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc60_all_body_t::song_t::_read() {
    m_step_count = m__io->read_u1();
    switch (step_count()) {
    case 0: {
        m_body = std::unique_ptr<empty_song_t>(new empty_song_t(m__io, this, m__root));
        break;
    }
    default: {
        m_body = std::unique_ptr<song_body_t>(new song_body_t(m__io, this, m__root));
        break;
    }
    }
}

void mpc60_all_body_t::song_t::_fetch_instances() {
    switch (step_count()) {
    case 0: {
        {
            empty_song_t* _switch_obj = dynamic_cast<empty_song_t*>(m_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    default: {
        {
            song_body_t* _switch_obj = dynamic_cast<song_body_t*>(m_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    }
}

mpc60_all_body_t::song_t::~song_t() {}

mpc60_all_body_t::song_body_t::song_body_t(kaitai::kstream* p__io, mpc60_all_body_t::song_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc60_all_body_t::song_body_t::_read() {
    m_song_number = m__io->read_u1();
    m_reserved_1 = m__io->read_bytes(1);
    if (!(m_reserved_1 == std::string("\x00", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00", 1), m_reserved_1, m__io, std::string("/types/song_body/seq/1"));
    }
    m_reserved_2 = m__io->read_bytes(1);
    if (!(m_reserved_2 == std::string("\x01", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01", 1), m_reserved_2, m__io, std::string("/types/song_body/seq/2"));
    }
    m_song_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m_reserved_3 = m__io->read_bytes(5);
    if (!(m_reserved_3 == std::string("\x00\x00\x00\x00\x00", 5))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00\x00\x00\x00", 5), m_reserved_3, m__io, std::string("/types/song_body/seq/4"));
    }
    m_steps = std::unique_ptr<std::vector<std::unique_ptr<song_step_t>>>(new std::vector<std::unique_ptr<song_step_t>>());
    const int l_steps = static_cast<mpc60_all_body_t::song_t*>(_parent())->step_count();
    for (int i = 0; i < l_steps; i++) {
        m_steps->push_back(std::move(std::unique_ptr<song_step_t>(new song_step_t(m__io, this, m__root))));
    }
}

void mpc60_all_body_t::song_body_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_steps->size(); ++i) {
        m_steps->at(i).get()->_fetch_instances();
    }
}

mpc60_all_body_t::song_body_t::~song_body_t() {}

mpc60_all_body_t::song_step_t::song_step_t(kaitai::kstream* p__io, mpc60_all_body_t::song_body_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc60_all_body_t::song_step_t::_read() {
    m_sequence_number = m__io->read_u1();
    m_repeats = m__io->read_u1();
}

void mpc60_all_body_t::song_step_t::_fetch_instances() {
}

mpc60_all_body_t::song_step_t::~song_step_t() {}

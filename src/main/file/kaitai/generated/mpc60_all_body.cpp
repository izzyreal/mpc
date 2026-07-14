// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_all_body.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>

mpc60_all_body_t::mpc60_all_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc60_all_body_t::_read() {
    m__dirty = false;
}

void mpc60_all_body_t::_fetch_instances() {
}

void mpc60_all_body_t::_write() {
    _fetch_instances();
    m__dirty = false;
}

void mpc60_all_body_t::_check() {
    m__dirty = false;
}

mpc60_all_body_t::~mpc60_all_body_t() {}

mpc60_all_body_t::all_file_body_t::all_file_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_all_body_t::all_file_body_t::_read() {
    m_total_number_of_bytes_in_all_sequences = m__io->read_u4le();
    m_sequences = std::unique_ptr<std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>>(new std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>());
    {
        int i = 0;
        mpc60_seq_body_t::sequence_t* _;
        do {
            std::unique_ptr<mpc60_seq_body_t::sequence_t> _t_sequences = std::unique_ptr<mpc60_seq_body_t::sequence_t>(new mpc60_seq_body_t::sequence_t(m__io));
            try {
                _t_sequences->_read();
            } catch(...) {
                _ = _t_sequences.get();
                m_sequences->push_back(std::move(_t_sequences));
                throw;
            }
            _ = _t_sequences.get();
            m_sequences->push_back(std::move(_t_sequences));
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
            std::unique_ptr<song_t> _t_songs = std::unique_ptr<song_t>(new song_t(m__io, this, m__root));
            try {
                _t_songs->_read();
            } catch(...) {
                _ = _t_songs.get();
                m_songs->push_back(std::move(_t_songs));
                throw;
            }
            _ = _t_songs.get();
            m_songs->push_back(std::move(_t_songs));
            i++;
        } while (!(_->step_count() == 0));
    }
    m__dirty = false;
}

void mpc60_all_body_t::all_file_body_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_sequences->size(); ++i) {
        m_sequences->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_songs->size(); ++i) {
        m_songs->at(i).get()->_fetch_instances();
    }
}

void mpc60_all_body_t::all_file_body_t::_write() {
    m__io->write_u4le(m_total_number_of_bytes_in_all_sequences);
    if (m_sequences == nullptr) {
        throw std::runtime_error("/types/all_file_body/seq/1: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>::const_iterator it = m_sequences->begin(); it != m_sequences->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/all_file_body/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m_sequences_terminator);
    if (m_songs == nullptr) {
        throw std::runtime_error("/types/all_file_body/seq/3: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<song_t>>::const_iterator it = m_songs->begin(); it != m_songs->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/all_file_body/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc60_all_body_t::all_file_body_t::_check() {
    if (m_sequences == nullptr) {
        throw std::runtime_error("/types/all_file_body/seq/1: repeated field is not set");
    }
    if (m_sequences->empty()) {
        throw std::runtime_error("/types/all_file_body/seq/1: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<mpc60_seq_body_t::sequence_t>>::const_iterator it = m_sequences->begin(); it != m_sequences->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_sequences->begin());
        const mpc60_seq_body_t::sequence_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/all_file_body/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_sequences->size() - 1);
        if ((_io()->pos() - 6 >= total_number_of_bytes_in_all_sequences() - 1) != _is_last) {
            throw std::runtime_error("/types/all_file_body/seq/1: repeat-until condition mismatch");
        }
    }
    if (m_sequences_terminator.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/all_file_body/seq/2: size mismatch");
    }
    if (!(m_sequences_terminator == std::string("\xFF", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\xFF", 1), m_sequences_terminator, m__io, std::string("/types/all_file_body/seq/2"));
    }
    if (m_songs == nullptr) {
        throw std::runtime_error("/types/all_file_body/seq/3: repeated field is not set");
    }
    if (m_songs->empty()) {
        throw std::runtime_error("/types/all_file_body/seq/3: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<song_t>>::const_iterator it = m_songs->begin(); it != m_songs->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_songs->begin());
        const song_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/all_file_body/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_songs->size() - 1);
        if ((_->step_count() == 0) != _is_last) {
            throw std::runtime_error("/types/all_file_body/seq/3: repeat-until condition mismatch");
        }
    }
    m__dirty = false;
}

mpc60_all_body_t::all_file_body_t::~all_file_body_t() {}

mpc60_all_body_t::empty_song_t::empty_song_t(kaitai::kstream* p__io, mpc60_all_body_t::song_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_all_body_t::empty_song_t::_read() {
    m__dirty = false;
}

void mpc60_all_body_t::empty_song_t::_fetch_instances() {
}

void mpc60_all_body_t::empty_song_t::_write() {
    _fetch_instances();
    m__dirty = false;
}

void mpc60_all_body_t::empty_song_t::_check() {
    m__dirty = false;
}

mpc60_all_body_t::empty_song_t::~empty_song_t() {}

mpc60_all_body_t::song_t::song_t(kaitai::kstream* p__io, mpc60_all_body_t::all_file_body_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_all_body_t::song_t::_read() {
    m_step_count = m__io->read_u1();
    switch (step_count()) {
    case 0: {
        m_body = std::unique_ptr<empty_song_t>(new empty_song_t(m__io, this, m__root));
        static_cast<empty_song_t*>(m_body.get())->_read();
        break;
    }
    default: {
        m_body = std::unique_ptr<song_body_t>(new song_body_t(m__io, this, m__root));
        static_cast<song_body_t*>(m_body.get())->_read();
        break;
    }
    }
    m__dirty = false;
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

void mpc60_all_body_t::song_t::_write() {
    m__io->write_u1(m_step_count);
    switch (step_count()) {
    case 0: {
        {
            empty_song_t* _switch_obj = dynamic_cast<empty_song_t*>(m_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    default: {
        {
            song_body_t* _switch_obj = dynamic_cast<song_body_t*>(m_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc60_all_body_t::song_t::_check() {
    switch (step_count()) {
    case 0: {
        {
            empty_song_t* _switch_obj = dynamic_cast<empty_song_t*>(m_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    default: {
        {
            song_body_t* _switch_obj = dynamic_cast<song_body_t*>(m_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/song/seq/1: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    }
    m__dirty = false;
}

mpc60_all_body_t::song_t::~song_t() {}

mpc60_all_body_t::song_body_t::song_body_t(kaitai::kstream* p__io, mpc60_all_body_t::song_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
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
        std::unique_ptr<song_step_t> _t_steps = std::unique_ptr<song_step_t>(new song_step_t(m__io, this, m__root));
        try {
            _t_steps->_read();
        } catch(...) {
            m_steps->push_back(std::move(_t_steps));
            throw;
        }
        m_steps->push_back(std::move(_t_steps));
    }
    m__dirty = false;
}

void mpc60_all_body_t::song_body_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_steps->size(); ++i) {
        m_steps->at(i).get()->_fetch_instances();
    }
}

void mpc60_all_body_t::song_body_t::_write() {
    m__io->write_u1(m_song_number);
    m__io->write_bytes(m_reserved_1);
    m__io->write_bytes(m_reserved_2);
    m__io->write_bytes(m_song_name);
    m__io->write_bytes(m_reserved_3);
    if (m_steps == nullptr) {
        throw std::runtime_error("/types/song_body/seq/5: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<song_step_t>>::const_iterator it = m_steps->begin(); it != m_steps->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/song_body/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc60_all_body_t::song_body_t::_check() {
    if (m_reserved_1.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/song_body/seq/1: size mismatch");
    }
    if (!(m_reserved_1 == std::string("\x00", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00", 1), m_reserved_1, m__io, std::string("/types/song_body/seq/1"));
    }
    if (m_reserved_2.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/song_body/seq/2: size mismatch");
    }
    if (!(m_reserved_2 == std::string("\x01", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x01", 1), m_reserved_2, m__io, std::string("/types/song_body/seq/2"));
    }
    if (m_song_name.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/song_body/seq/3: size mismatch");
    }
    if (m_reserved_3.size() != static_cast<std::string::size_type>(5)) {
        throw std::runtime_error("/types/song_body/seq/4: size mismatch");
    }
    if (!(m_reserved_3 == std::string("\x00\x00\x00\x00\x00", 5))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00\x00\x00\x00", 5), m_reserved_3, m__io, std::string("/types/song_body/seq/4"));
    }
    if (m_steps == nullptr) {
        throw std::runtime_error("/types/song_body/seq/5: repeated field is not set");
    }
    if (m_steps->size() != static_cast<std::size_t>(static_cast<mpc60_all_body_t::song_t*>(_parent())->step_count())) {
        throw std::runtime_error("/types/song_body/seq/5: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<song_step_t>>::const_iterator it = m_steps->begin(); it != m_steps->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/song_body/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

mpc60_all_body_t::song_body_t::~song_body_t() {}

mpc60_all_body_t::song_step_t::song_step_t(kaitai::kstream* p__io, mpc60_all_body_t::song_body_t* p__parent, mpc60_all_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_all_body_t::song_step_t::_read() {
    m_sequence_number = m__io->read_u1();
    m_repeats = m__io->read_u1();
    m__dirty = false;
}

void mpc60_all_body_t::song_step_t::_fetch_instances() {
}

void mpc60_all_body_t::song_step_t::_write() {
    m__io->write_u1(m_sequence_number);
    m__io->write_u1(m_repeats);
    _fetch_instances();
    m__dirty = false;
}

void mpc60_all_body_t::song_step_t::_check() {
    m__dirty = false;
}

mpc60_all_body_t::song_step_t::~song_step_t() {}

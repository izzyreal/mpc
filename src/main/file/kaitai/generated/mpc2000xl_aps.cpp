// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc2000xl_aps.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>
const std::set<mpc2000xl_aps_t::mix_source_t> mpc2000xl_aps_t::_values_mix_source_t{
    mpc2000xl_aps_t::MIX_SOURCE_PROGRAM,
    mpc2000xl_aps_t::MIX_SOURCE_DRUM,
};
bool mpc2000xl_aps_t::_is_defined_mix_source_t(mpc2000xl_aps_t::mix_source_t v) {
    return mpc2000xl_aps_t::_values_mix_source_t.find(v) != mpc2000xl_aps_t::_values_mix_source_t.end();
}
const std::set<mpc2000xl_aps_t::no_yes_t> mpc2000xl_aps_t::_values_no_yes_t{
    mpc2000xl_aps_t::NO_YES_FALSE,
    mpc2000xl_aps_t::NO_YES_TRUE,
};
bool mpc2000xl_aps_t::_is_defined_no_yes_t(mpc2000xl_aps_t::no_yes_t v) {
    return mpc2000xl_aps_t::_values_no_yes_t.find(v) != mpc2000xl_aps_t::_values_no_yes_t.end();
}
const std::set<mpc2000xl_aps_t::pad_assign_t> mpc2000xl_aps_t::_values_pad_assign_t{
    mpc2000xl_aps_t::PAD_ASSIGN_PROGRAM,
    mpc2000xl_aps_t::PAD_ASSIGN_MASTERS,
};
bool mpc2000xl_aps_t::_is_defined_pad_assign_t(mpc2000xl_aps_t::pad_assign_t v) {
    return mpc2000xl_aps_t::_values_pad_assign_t.find(v) != mpc2000xl_aps_t::_values_pad_assign_t.end();
}

mpc2000xl_aps_t::mpc2000xl_aps_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc2000xl_aps_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc2000xl_aps_t::_read() {
    m_magic = m__io->read_bytes(2);
    if (!(m_magic == std::string("\x0A\x05", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x0A\x05", 2), m_magic, m__io, std::string("/seq/0"));
    }
    m_sound_count = m__io->read_u1();
    m__unnamed2 = m__io->read_bytes(1);
    m_sound_names = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    const int l_sound_names = sound_count();
    for (int i = 0; i < l_sound_names; i++) {
        m_sound_names->push_back(std::move(kaitai::kstream::bytes_to_str(m__io->read_bytes(17), "ASCII")));
    }
    m__unnamed4 = m__io->read_bytes(2);
    m_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(17), "ASCII");
    m_global_parameters = std::unique_ptr<global_parameters_t>(new global_parameters_t(m__io, this, m__root));
    m_global_parameters->_read();
    m__unnamed7 = m__io->read_bytes(1);
    m_master_pad_to_note_mapping = std::unique_ptr<std::vector<int8_t>>(new std::vector<int8_t>());
    const int l_master_pad_to_note_mapping = 64;
    for (int i = 0; i < l_master_pad_to_note_mapping; i++) {
        m_master_pad_to_note_mapping->push_back(std::move(m__io->read_s1()));
    }
    m__unnamed9 = m__io->read_bytes(7);
    m_drum1 = std::unique_ptr<drum_t>(new drum_t(m__io, this, m__root));
    m_drum1->_read();
    m__unnamed11 = m__io->read_bytes(4);
    m_drum2 = std::unique_ptr<drum_t>(new drum_t(m__io, this, m__root));
    m_drum2->_read();
    m__unnamed13 = m__io->read_bytes(4);
    m_drum3 = std::unique_ptr<drum_t>(new drum_t(m__io, this, m__root));
    m_drum3->_read();
    m__unnamed15 = m__io->read_bytes(4);
    m_drum4 = std::unique_ptr<drum_t>(new drum_t(m__io, this, m__root));
    m_drum4->_read();
    m__unnamed17 = m__io->read_bytes(1);
    m_aps_programs = std::unique_ptr<std::vector<std::unique_ptr<aps_program_meta_t>>>(new std::vector<std::unique_ptr<aps_program_meta_t>>());
    {
        int i = 0;
        while (!m__io->is_eof()) {
            std::unique_ptr<aps_program_meta_t> _t_aps_programs = std::unique_ptr<aps_program_meta_t>(new aps_program_meta_t(m__io, this, m__root));
            try {
                _t_aps_programs->_read();
            } catch(...) {
                m_aps_programs->push_back(std::move(_t_aps_programs));
                throw;
            }
            m_aps_programs->push_back(std::move(_t_aps_programs));
            i++;
        }
    }
    m__dirty = false;
}

void mpc2000xl_aps_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_sound_names->size(); ++i) {
    }
    m_global_parameters.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_master_pad_to_note_mapping->size(); ++i) {
    }
    m_drum1.get()->_fetch_instances();
    m_drum2.get()->_fetch_instances();
    m_drum3.get()->_fetch_instances();
    m_drum4.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_aps_programs->size(); ++i) {
        m_aps_programs->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_aps_t::_write() {
    m__io->write_bytes(m_magic);
    m__io->write_u1(m_sound_count);
    m__io->write_bytes(m__unnamed2);
    if (m_sound_names == nullptr) {
        throw std::runtime_error("/seq/3: repeated field is not set");
    }
    for (std::vector<std::string>::const_iterator it = m_sound_names->begin(); it != m_sound_names->end(); ++it) {
        m__io->write_bytes((*it));
    }
    m__io->write_bytes(m__unnamed4);
    m__io->write_bytes(m_name);
    if (m_global_parameters.get() == nullptr) {
        throw std::runtime_error("/seq/6: nested object is not set");
    }
    m_global_parameters.get()->_set_io(m__io);
    m_global_parameters.get()->_write();
    m__io->write_bytes(m__unnamed7);
    if (m_master_pad_to_note_mapping == nullptr) {
        throw std::runtime_error("/seq/8: repeated field is not set");
    }
    for (std::vector<int8_t>::const_iterator it = m_master_pad_to_note_mapping->begin(); it != m_master_pad_to_note_mapping->end(); ++it) {
        m__io->write_s1((*it));
    }
    m__io->write_bytes(m__unnamed9);
    if (m_drum1.get() == nullptr) {
        throw std::runtime_error("/seq/10: nested object is not set");
    }
    m_drum1.get()->_set_io(m__io);
    m_drum1.get()->_write();
    m__io->write_bytes(m__unnamed11);
    if (m_drum2.get() == nullptr) {
        throw std::runtime_error("/seq/12: nested object is not set");
    }
    m_drum2.get()->_set_io(m__io);
    m_drum2.get()->_write();
    m__io->write_bytes(m__unnamed13);
    if (m_drum3.get() == nullptr) {
        throw std::runtime_error("/seq/14: nested object is not set");
    }
    m_drum3.get()->_set_io(m__io);
    m_drum3.get()->_write();
    m__io->write_bytes(m__unnamed15);
    if (m_drum4.get() == nullptr) {
        throw std::runtime_error("/seq/16: nested object is not set");
    }
    m_drum4.get()->_set_io(m__io);
    m_drum4.get()->_write();
    m__io->write_bytes(m__unnamed17);
    if (m_aps_programs == nullptr) {
        throw std::runtime_error("/seq/18: repeated field is not set");
    }
    if (m_aps_programs == nullptr) {
        throw std::runtime_error("/seq/18: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<aps_program_meta_t>>::const_iterator it = m_aps_programs->begin(); it != m_aps_programs->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/18: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_aps_t::_check() {
    if (m_magic.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/seq/0: size mismatch");
    }
    if (!(m_magic == std::string("\x0A\x05", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x0A\x05", 2), m_magic, m__io, std::string("/seq/0"));
    }
    if (m__unnamed2.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/2: size mismatch");
    }
    if (m_sound_names == nullptr) {
        throw std::runtime_error("/seq/3: repeated field is not set");
    }
    if (m_sound_names->size() != static_cast<std::size_t>(sound_count())) {
        throw std::runtime_error("/seq/3: repeat-expr size mismatch");
    }
    for (std::vector<std::string>::const_iterator it = m_sound_names->begin(); it != m_sound_names->end(); ++it) {
        if ((*it).size() != static_cast<std::string::size_type>(17)) {
            throw std::runtime_error("/seq/3: size mismatch");
        }
    }
    if (m__unnamed4.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/seq/4: size mismatch");
    }
    if (m_name.size() != static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/seq/5: size mismatch");
    }
    if (m_global_parameters.get() == nullptr) {
        throw std::runtime_error("/seq/6: nested object is not set");
    }
    m_global_parameters.get()->_set_io(m__io);
    m_global_parameters.get()->_check();
    if (m__unnamed7.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/7: size mismatch");
    }
    if (m_master_pad_to_note_mapping == nullptr) {
        throw std::runtime_error("/seq/8: repeated field is not set");
    }
    if (m_master_pad_to_note_mapping->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/seq/8: repeat-expr size mismatch");
    }
    for (std::vector<int8_t>::const_iterator it = m_master_pad_to_note_mapping->begin(); it != m_master_pad_to_note_mapping->end(); ++it) {
    }
    if (m__unnamed9.size() != static_cast<std::string::size_type>(7)) {
        throw std::runtime_error("/seq/9: size mismatch");
    }
    if (m_drum1.get() == nullptr) {
        throw std::runtime_error("/seq/10: nested object is not set");
    }
    m_drum1.get()->_set_io(m__io);
    m_drum1.get()->_check();
    if (m__unnamed11.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/seq/11: size mismatch");
    }
    if (m_drum2.get() == nullptr) {
        throw std::runtime_error("/seq/12: nested object is not set");
    }
    m_drum2.get()->_set_io(m__io);
    m_drum2.get()->_check();
    if (m__unnamed13.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/seq/13: size mismatch");
    }
    if (m_drum3.get() == nullptr) {
        throw std::runtime_error("/seq/14: nested object is not set");
    }
    m_drum3.get()->_set_io(m__io);
    m_drum3.get()->_check();
    if (m__unnamed15.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/seq/15: size mismatch");
    }
    if (m_drum4.get() == nullptr) {
        throw std::runtime_error("/seq/16: nested object is not set");
    }
    m_drum4.get()->_set_io(m__io);
    m_drum4.get()->_check();
    if (m__unnamed17.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/17: size mismatch");
    }
    if (m_aps_programs == nullptr) {
        throw std::runtime_error("/seq/18: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<aps_program_meta_t>>::const_iterator it = m_aps_programs->begin(); it != m_aps_programs->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_aps_programs->begin());
        const aps_program_meta_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/18: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

mpc2000xl_aps_t::~mpc2000xl_aps_t() {}

mpc2000xl_aps_t::aps_program_body_t::aps_program_body_t(kaitai::kstream* p__io, mpc2000xl_aps_t::aps_program_meta_t* p__parent, mpc2000xl_aps_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_aps_t::aps_program_body_t::_read() {
    m__unnamed0 = m__io->read_bytes(5);
    m_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(17), "ASCII");
    m_slider = std::unique_ptr<mpc2000xl_pgm_t::slider_t>(new mpc2000xl_pgm_t::slider_t(m__io));
    m_slider->_read();
    m_program_change = m__io->read_u1();
    m__unnamed4 = m__io->read_bytes(5);
    m_note_parameters = std::unique_ptr<std::vector<std::unique_ptr<note_t>>>(new std::vector<std::unique_ptr<note_t>>());
    const int l_note_parameters = 64;
    for (int i = 0; i < l_note_parameters; i++) {
        std::unique_ptr<note_t> _t_note_parameters = std::unique_ptr<note_t>(new note_t(m__io, this, m__root));
        try {
            _t_note_parameters->_read();
        } catch(...) {
            m_note_parameters->push_back(std::move(_t_note_parameters));
            throw;
        }
        m_note_parameters->push_back(std::move(_t_note_parameters));
    }
    m__unnamed6 = m__io->read_bytes(1);
    m_pad_mixers = std::unique_ptr<std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>>(new std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>());
    const int l_pad_mixers = 64;
    for (int i = 0; i < l_pad_mixers; i++) {
        std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t> _t_pad_mixers = std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>(new mpc2000xl_pgm_t::pad_mixer_t(m__io));
        try {
            _t_pad_mixers->_read();
        } catch(...) {
            m_pad_mixers->push_back(std::move(_t_pad_mixers));
            throw;
        }
        m_pad_mixers->push_back(std::move(_t_pad_mixers));
    }
    m__unnamed8 = m__io->read_bytes(3);
    m_pad_to_note_mapping = std::unique_ptr<std::vector<int8_t>>(new std::vector<int8_t>());
    const int l_pad_to_note_mapping = 64;
    for (int i = 0; i < l_pad_to_note_mapping; i++) {
        m_pad_to_note_mapping->push_back(std::move(m__io->read_s1()));
    }
    m__unnamed10 = m__io->read_bytes(200);
    m__dirty = false;
}

void mpc2000xl_aps_t::aps_program_body_t::_fetch_instances() {
    m_slider.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_note_parameters->size(); ++i) {
        m_note_parameters->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_pad_mixers->size(); ++i) {
        m_pad_mixers->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_pad_to_note_mapping->size(); ++i) {
    }
}

void mpc2000xl_aps_t::aps_program_body_t::_write() {
    m__io->write_bytes(m__unnamed0);
    m__io->write_bytes(m_name);
    if (m_slider.get() == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/2: nested object is not set");
    }
    m_slider.get()->_set_io(m__io);
    m_slider.get()->_write();
    m__io->write_u1(m_program_change);
    m__io->write_bytes(m__unnamed4);
    if (m_note_parameters == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/5: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<note_t>>::const_iterator it = m_note_parameters->begin(); it != m_note_parameters->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/aps_program_body/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed6);
    if (m_pad_mixers == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/7: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>::const_iterator it = m_pad_mixers->begin(); it != m_pad_mixers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/aps_program_body/seq/7: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed8);
    if (m_pad_to_note_mapping == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/9: repeated field is not set");
    }
    for (std::vector<int8_t>::const_iterator it = m_pad_to_note_mapping->begin(); it != m_pad_to_note_mapping->end(); ++it) {
        m__io->write_s1((*it));
    }
    m__io->write_bytes(m__unnamed10);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_aps_t::aps_program_body_t::_check() {
    if (m__unnamed0.size() != static_cast<std::string::size_type>(5)) {
        throw std::runtime_error("/types/aps_program_body/seq/0: size mismatch");
    }
    if (m_name.size() != static_cast<std::string::size_type>(17)) {
        throw std::runtime_error("/types/aps_program_body/seq/1: size mismatch");
    }
    if (m_slider.get() == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/2: nested object is not set");
    }
    m_slider.get()->_set_io(m__io);
    m_slider.get()->_check();
    if (m__unnamed4.size() != static_cast<std::string::size_type>(5)) {
        throw std::runtime_error("/types/aps_program_body/seq/4: size mismatch");
    }
    if (m_note_parameters == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/5: repeated field is not set");
    }
    if (m_note_parameters->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/aps_program_body/seq/5: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<note_t>>::const_iterator it = m_note_parameters->begin(); it != m_note_parameters->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/aps_program_body/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed6.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/aps_program_body/seq/6: size mismatch");
    }
    if (m_pad_mixers == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/7: repeated field is not set");
    }
    if (m_pad_mixers->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/aps_program_body/seq/7: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>::const_iterator it = m_pad_mixers->begin(); it != m_pad_mixers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/aps_program_body/seq/7: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed8.size() != static_cast<std::string::size_type>(3)) {
        throw std::runtime_error("/types/aps_program_body/seq/8: size mismatch");
    }
    if (m_pad_to_note_mapping == nullptr) {
        throw std::runtime_error("/types/aps_program_body/seq/9: repeated field is not set");
    }
    if (m_pad_to_note_mapping->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/aps_program_body/seq/9: repeat-expr size mismatch");
    }
    for (std::vector<int8_t>::const_iterator it = m_pad_to_note_mapping->begin(); it != m_pad_to_note_mapping->end(); ++it) {
    }
    if (m__unnamed10.size() != static_cast<std::string::size_type>(200)) {
        throw std::runtime_error("/types/aps_program_body/seq/10: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_aps_t::aps_program_body_t::~aps_program_body_t() {}

mpc2000xl_aps_t::aps_program_meta_t::aps_program_meta_t(kaitai::kstream* p__io, mpc2000xl_aps_t* p__parent, mpc2000xl_aps_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_aps_t::aps_program_meta_t::_read() {
    m_index = m__io->read_u1();
    if (index() < 24) {
        m_body = std::unique_ptr<aps_program_body_t>(new aps_program_body_t(m__io, this, m__root));
        m_body->_read();
    }
    if (index() > 23) {
        m__unnamed2 = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
        {
            int i = 0;
            while (!m__io->is_eof()) {
                m__unnamed2->push_back(std::move(m__io->read_u1()));
                i++;
            }
        }
    }
    m__dirty = false;
}

void mpc2000xl_aps_t::aps_program_meta_t::_fetch_instances() {
    if (index() < 24) {
        m_body.get()->_fetch_instances();
    }
    if (index() > 23) {
        for (std::size_t i = 0; i < m__unnamed2->size(); ++i) {
        }
    }
}

void mpc2000xl_aps_t::aps_program_meta_t::_write() {
    m__io->write_u1(m_index);
    if (index() < 24) {
        if (m_body.get() == nullptr) {
            throw std::runtime_error("/types/aps_program_meta/seq/1: nested object is not set");
        }
        m_body.get()->_set_io(m__io);
        m_body.get()->_write();
    }
    if (index() > 23) {
        if (m__unnamed2 == nullptr) {
            throw std::runtime_error("/types/aps_program_meta/seq/2: repeated field is not set");
        }
        const std::size_t _remaining = static_cast<std::size_t>(m__io->size() - m__io->pos());
        const std::size_t _actual = m__unnamed2->size() * static_cast<std::size_t>(1);
        if (_remaining != 0 && _remaining != _actual) {
            throw std::runtime_error("/types/aps_program_meta/seq/2: expected: " + kaitai::kstream::to_string(_remaining) + ", actual: " + kaitai::kstream::to_string(_actual));
        }
        if (m__unnamed2 == nullptr) {
            throw std::runtime_error("/types/aps_program_meta/seq/2: repeated field is not set");
        }
        for (std::vector<uint8_t>::const_iterator it = m__unnamed2->begin(); it != m__unnamed2->end(); ++it) {
            m__io->write_u1((*it));
        }
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_aps_t::aps_program_meta_t::_check() {
    if (index() < 24) {
        if (!(m_body.get() != nullptr)) {
            throw std::runtime_error("/types/aps_program_meta/seq/1: conditional field is not set");
        }
        if (m_body.get() == nullptr) {
            throw std::runtime_error("/types/aps_program_meta/seq/1: nested object is not set");
        }
        m_body.get()->_set_io(m__io);
        m_body.get()->_check();
    } else {
        if (m_body.get() != nullptr) {
            throw std::runtime_error("/types/aps_program_meta/seq/1: conditional field should be absent");
        }
    }
    if (index() > 23) {
        if (!(m__unnamed2 != nullptr)) {
            throw std::runtime_error("/types/aps_program_meta/seq/2: conditional field is not set");
        }
        if (m__unnamed2 == nullptr) {
            throw std::runtime_error("/types/aps_program_meta/seq/2: repeated field is not set");
        }
        for (std::vector<uint8_t>::const_iterator it = m__unnamed2->begin(); it != m__unnamed2->end(); ++it) {
            const std::size_t i = static_cast<std::size_t>(it - m__unnamed2->begin());
            const uint8_t _ = (*it);
        }
    } else {
        if (m__unnamed2 != nullptr) {
            throw std::runtime_error("/types/aps_program_meta/seq/2: conditional field should be absent");
        }
    }
    m__dirty = false;
}

mpc2000xl_aps_t::aps_program_meta_t::~aps_program_meta_t() {}

mpc2000xl_aps_t::drum_t::drum_t(kaitai::kstream* p__io, mpc2000xl_aps_t* p__parent, mpc2000xl_aps_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_aps_t::drum_t::_read() {
    m_pad_mixers = std::unique_ptr<std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>>(new std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>());
    const int l_pad_mixers = 64;
    for (int i = 0; i < l_pad_mixers; i++) {
        std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t> _t_pad_mixers = std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>(new mpc2000xl_pgm_t::pad_mixer_t(m__io));
        try {
            _t_pad_mixers->_read();
        } catch(...) {
            m_pad_mixers->push_back(std::move(_t_pad_mixers));
            throw;
        }
        m_pad_mixers->push_back(std::move(_t_pad_mixers));
    }
    m__unnamed1 = m__io->read_bytes(2);
    m_receive_program_change = static_cast<mpc2000xl_aps_t::no_yes_t>(m__io->read_u1());
    m_receive_midi_volume = static_cast<mpc2000xl_aps_t::no_yes_t>(m__io->read_u1());
    m__unnamed4 = m__io->read_bytes(1);
    m_program = m__io->read_u1();
    m_receive_program_change_duplicate = static_cast<mpc2000xl_aps_t::no_yes_t>(m__io->read_u1());
    m_receive_midi_volume_duplicate = static_cast<mpc2000xl_aps_t::no_yes_t>(m__io->read_u1());
    m__dirty = false;
}

void mpc2000xl_aps_t::drum_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_pad_mixers->size(); ++i) {
        m_pad_mixers->at(i).get()->_fetch_instances();
    }
}

void mpc2000xl_aps_t::drum_t::_write() {
    if (m_pad_mixers == nullptr) {
        throw std::runtime_error("/types/drum/seq/0: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>::const_iterator it = m_pad_mixers->begin(); it != m_pad_mixers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/drum/seq/0: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m__unnamed1);
    m__io->write_u1(static_cast<uint8_t>(m_receive_program_change));
    m__io->write_u1(static_cast<uint8_t>(m_receive_midi_volume));
    m__io->write_bytes(m__unnamed4);
    m__io->write_u1(m_program);
    m__io->write_u1(static_cast<uint8_t>(m_receive_program_change_duplicate));
    m__io->write_u1(static_cast<uint8_t>(m_receive_midi_volume_duplicate));
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_aps_t::drum_t::_check() {
    if (m_pad_mixers == nullptr) {
        throw std::runtime_error("/types/drum/seq/0: repeated field is not set");
    }
    if (m_pad_mixers->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/drum/seq/0: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>::const_iterator it = m_pad_mixers->begin(); it != m_pad_mixers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/drum/seq/0: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m__unnamed1.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/types/drum/seq/1: size mismatch");
    }
    if (m__unnamed4.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/drum/seq/4: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_aps_t::drum_t::~drum_t() {}

mpc2000xl_aps_t::global_parameters_t::global_parameters_t(kaitai::kstream* p__io, mpc2000xl_aps_t* p__parent, mpc2000xl_aps_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_aps_t::global_parameters_t::_read() {
    m_pad_to_internal_sound = static_cast<mpc2000xl_aps_t::no_yes_t>(m__io->read_bits_int_le(1));
    m__unnamed1 = m__io->read_bits_int_le(7);
    m_pad_assign = static_cast<mpc2000xl_aps_t::pad_assign_t>(m__io->read_bits_int_le(1));
    m__unnamed3 = m__io->read_bits_int_le(7);
    m_stereo_mix_source = static_cast<mpc2000xl_aps_t::mix_source_t>(m__io->read_bits_int_le(1));
    m_indiv_fx_source = static_cast<mpc2000xl_aps_t::mix_source_t>(m__io->read_bits_int_le(1));
    m__unnamed6 = m__io->read_bits_int_le(6);
    m_copy_pgm_mix_to_drum = static_cast<mpc2000xl_aps_t::no_yes_t>(m__io->read_bits_int_le(1));
    m__unnamed8 = m__io->read_bits_int_le(3);
    m_record_mix_changes = static_cast<mpc2000xl_aps_t::no_yes_t>(m__io->read_bits_int_le(1));
    m__unnamed10 = m__io->read_bits_int_le(3);
    m_fx_drum = m__io->read_u1();
    m__unnamed12 = m__io->read_bytes(1);
    m_master_level = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_aps_t::global_parameters_t::_fetch_instances() {
}

void mpc2000xl_aps_t::global_parameters_t::_write() {
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_pad_to_internal_sound));
    m__io->write_bits_int_le(7, m__unnamed1);
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_pad_assign));
    m__io->write_bits_int_le(7, m__unnamed3);
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_stereo_mix_source));
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_indiv_fx_source));
    m__io->write_bits_int_le(6, m__unnamed6);
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_copy_pgm_mix_to_drum));
    m__io->write_bits_int_le(3, m__unnamed8);
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_record_mix_changes));
    m__io->write_bits_int_le(3, m__unnamed10);
    m__io->write_u1(m_fx_drum);
    m__io->write_bytes(m__unnamed12);
    m__io->write_u1(m_master_level);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_aps_t::global_parameters_t::_check() {
    if (m__unnamed12.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/global_parameters/seq/12: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_aps_t::global_parameters_t::~global_parameters_t() {}

mpc2000xl_aps_t::note_t::note_t(kaitai::kstream* p__io, mpc2000xl_aps_t::aps_program_body_t* p__parent, mpc2000xl_aps_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_aps_t::note_t::_read() {
    m_sound_index = m__io->read_u1();
    m__unnamed1 = m__io->read_bytes(1);
    m_sound_generation_mode = static_cast<mpc2000xl_pgm_t::sound_generation_mode_t>(m__io->read_u1());
    m_velocity_range_lower = m__io->read_u1();
    m_also_play_use_note_1 = m__io->read_u1();
    m_velocity_range_upper = m__io->read_u1();
    m_also_play_use_note_2 = m__io->read_u1();
    m_voice_overlap_mode = static_cast<mpc2000xl_pgm_t::voice_overlap_mode_t>(m__io->read_u1());
    m_mute_assign_1 = m__io->read_u1();
    m_mute_assign_2 = m__io->read_u1();
    m_tune = m__io->read_s2le();
    m_attack = m__io->read_u1();
    m_decay = m__io->read_u1();
    m_decay_mode = static_cast<mpc2000xl_pgm_t::decay_mode_t>(m__io->read_u1());
    m_cutoff = m__io->read_u1();
    m_resonance = m__io->read_u1();
    m_velocity_envelope_to_filter_attack = m__io->read_u1();
    m_velocity_envelope_to_filter_decay = m__io->read_u1();
    m_velocity_envelope_to_filter_amount = m__io->read_u1();
    m_velocity_to_level = m__io->read_u1();
    m_velocity_to_attack = m__io->read_u1();
    m_velocity_to_start = m__io->read_u1();
    m_velocity_to_cutoff = m__io->read_u1();
    m_slider_parameter = static_cast<mpc2000xl_pgm_t::slider_parameter_t>(m__io->read_u1());
    m_velocity_to_pitch = m__io->read_u1();
    m__dirty = false;
}

void mpc2000xl_aps_t::note_t::_fetch_instances() {
}

void mpc2000xl_aps_t::note_t::_write() {
    m__io->write_u1(m_sound_index);
    m__io->write_bytes(m__unnamed1);
    m__io->write_u1(static_cast<uint8_t>(m_sound_generation_mode));
    m__io->write_u1(m_velocity_range_lower);
    m__io->write_u1(m_also_play_use_note_1);
    m__io->write_u1(m_velocity_range_upper);
    m__io->write_u1(m_also_play_use_note_2);
    m__io->write_u1(static_cast<uint8_t>(m_voice_overlap_mode));
    m__io->write_u1(m_mute_assign_1);
    m__io->write_u1(m_mute_assign_2);
    m__io->write_s2le(m_tune);
    m__io->write_u1(m_attack);
    m__io->write_u1(m_decay);
    m__io->write_u1(static_cast<uint8_t>(m_decay_mode));
    m__io->write_u1(m_cutoff);
    m__io->write_u1(m_resonance);
    m__io->write_u1(m_velocity_envelope_to_filter_attack);
    m__io->write_u1(m_velocity_envelope_to_filter_decay);
    m__io->write_u1(m_velocity_envelope_to_filter_amount);
    m__io->write_u1(m_velocity_to_level);
    m__io->write_u1(m_velocity_to_attack);
    m__io->write_u1(m_velocity_to_start);
    m__io->write_u1(m_velocity_to_cutoff);
    m__io->write_u1(static_cast<uint8_t>(m_slider_parameter));
    m__io->write_u1(m_velocity_to_pitch);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_aps_t::note_t::_check() {
    if (m__unnamed1.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/note/seq/1: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_aps_t::note_t::~note_t() {}

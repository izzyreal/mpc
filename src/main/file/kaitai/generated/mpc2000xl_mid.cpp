// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc2000xl_mid.h"
#include <cstddef>
#include <stdexcept>
#include "kaitai/exceptions.h"

mpc2000xl_mid_t::mpc2000xl_mid_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc2000xl_mid_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
    f_denominator = false;
    f_first_meta = false;
    f_is_mpc2000xl_mid = false;
    f_meta_events = false;
    f_numerator = false;
    f_sequence_name = false;
    f_sequencer = false;
    f_smpte_offset = false;
    f_tempo_bpm = false;
    w_sequencer = false;
    e_sequencer = true;
}

void mpc2000xl_mid_t::_read() {
    m_hdr = std::unique_ptr<standard_midi_file_with_running_status_t::header_t>(new standard_midi_file_with_running_status_t::header_t(m__io));
    m_hdr->_read();
    m_tracks = std::unique_ptr<std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_t>>>(new std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_t>>());
    const int l_tracks = hdr()->num_tracks();
    for (int i = 0; i < l_tracks; i++) {
        std::unique_ptr<standard_midi_file_with_running_status_t::track_t> _t_tracks = std::unique_ptr<standard_midi_file_with_running_status_t::track_t>(new standard_midi_file_with_running_status_t::track_t(m__io));
        try {
            _t_tracks->_read();
        } catch(...) {
            m_tracks->push_back(std::move(_t_tracks));
            throw;
        }
        m_tracks->push_back(std::move(_t_tracks));
    }
    m__dirty = false;
}

void mpc2000xl_mid_t::_fetch_instances() {
    m_hdr.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_tracks->size(); ++i) {
        m_tracks->at(i).get()->_fetch_instances();
    }
    sequencer();
    if (f_sequencer) {
        m_sequencer.get()->_fetch_instances();
    }
}

void mpc2000xl_mid_t::_write() {
    w_sequencer = e_sequencer;
    if (m_hdr.get() == nullptr) {
        throw std::runtime_error("/seq/0: nested object is not set");
    }
    m_hdr.get()->_set_io(m__io);
    m_hdr.get()->_write();
    if (m_tracks == nullptr) {
        throw std::runtime_error("/seq/1: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_t>>::const_iterator it = m_tracks->begin(); it != m_tracks->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_mid_t::_check() {
    if (m_hdr.get() == nullptr) {
        throw std::runtime_error("/seq/0: nested object is not set");
    }
    m_hdr.get()->_set_io(m__io);
    m_hdr.get()->_check();
    if (m_tracks == nullptr) {
        throw std::runtime_error("/seq/1: repeated field is not set");
    }
    if (m_tracks->size() != static_cast<std::size_t>(hdr()->num_tracks())) {
        throw std::runtime_error("/seq/1: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_t>>::const_iterator it = m_tracks->begin(); it != m_tracks->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (e_sequencer) {
        kaitai::kstream *io = tracks()->at(0)->events()->_io();
        if (m_sequencer.get() == nullptr) {
            throw std::runtime_error("/instances/sequencer: nested object is not set");
        }
        m_sequencer.get()->_set_io(io);
        m_sequencer.get()->_check();
    }
    m__dirty = false;
}

mpc2000xl_mid_t::~mpc2000xl_mid_t() {}

mpc2000xl_mid_t::sequencer_t::sequencer_t(kaitai::kstream* p__io, mpc2000xl_mid_t* p__parent, mpc2000xl_mid_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc2000xl_mid_t::sequencer_t::_read() {
    m__unnamed0 = m__io->read_bytes(5);
    if (!(m__unnamed0 == std::string("\x4C\x4F\x4F\x50\x3D", 5))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4C\x4F\x4F\x50\x3D", 5), m__unnamed0, m__io, std::string("/types/sequencer/seq/0"));
    }
    m_loop_enabled = kaitai::kstream::bytes_to_str(m__io->read_bytes(4), "ASCII");
    m__unnamed2 = m__io->read_bytes(6);
    if (!(m__unnamed2 == std::string("\x53\x54\x41\x52\x54\x3D", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x53\x54\x41\x52\x54\x3D", 6), m__unnamed2, m__io, std::string("/types/sequencer/seq/2"));
    }
    m_loop_start = kaitai::kstream::bytes_to_str(m__io->read_bytes(4), "ASCII");
    m__unnamed4 = m__io->read_bytes(4);
    if (!(m__unnamed4 == std::string("\x45\x4E\x44\x3D", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x45\x4E\x44\x3D", 4), m__unnamed4, m__io, std::string("/types/sequencer/seq/4"));
    }
    m_loop_end = kaitai::kstream::bytes_to_str(m__io->read_bytes(4), "ASCII");
    m__unnamed6 = m__io->read_bytes(6);
    if (!(m__unnamed6 == std::string("\x54\x45\x4D\x50\x4F\x3D", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x54\x45\x4D\x50\x4F\x3D", 6), m__unnamed6, m__io, std::string("/types/sequencer/seq/6"));
    }
    m_tempo_source = kaitai::kstream::bytes_to_str(m__io->read_bytes(3), "ASCII");
    m__dirty = false;
}

void mpc2000xl_mid_t::sequencer_t::_fetch_instances() {
}

void mpc2000xl_mid_t::sequencer_t::_write() {
    m__io->write_bytes(m__unnamed0);
    m__io->write_bytes(m_loop_enabled);
    m__io->write_bytes(m__unnamed2);
    m__io->write_bytes(m_loop_start);
    m__io->write_bytes(m__unnamed4);
    m__io->write_bytes(m_loop_end);
    m__io->write_bytes(m__unnamed6);
    m__io->write_bytes(m_tempo_source);
    _fetch_instances();
    m__dirty = false;
}

void mpc2000xl_mid_t::sequencer_t::_check() {
    if (m__unnamed0.size() != static_cast<std::string::size_type>(5)) {
        throw std::runtime_error("/types/sequencer/seq/0: size mismatch");
    }
    if (!(m__unnamed0 == std::string("\x4C\x4F\x4F\x50\x3D", 5))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4C\x4F\x4F\x50\x3D", 5), m__unnamed0, m__io, std::string("/types/sequencer/seq/0"));
    }
    if (m_loop_enabled.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/types/sequencer/seq/1: size mismatch");
    }
    if (m__unnamed2.size() != static_cast<std::string::size_type>(6)) {
        throw std::runtime_error("/types/sequencer/seq/2: size mismatch");
    }
    if (!(m__unnamed2 == std::string("\x53\x54\x41\x52\x54\x3D", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x53\x54\x41\x52\x54\x3D", 6), m__unnamed2, m__io, std::string("/types/sequencer/seq/2"));
    }
    if (m_loop_start.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/types/sequencer/seq/3: size mismatch");
    }
    if (m__unnamed4.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/types/sequencer/seq/4: size mismatch");
    }
    if (!(m__unnamed4 == std::string("\x45\x4E\x44\x3D", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x45\x4E\x44\x3D", 4), m__unnamed4, m__io, std::string("/types/sequencer/seq/4"));
    }
    if (m_loop_end.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/types/sequencer/seq/5: size mismatch");
    }
    if (m__unnamed6.size() != static_cast<std::string::size_type>(6)) {
        throw std::runtime_error("/types/sequencer/seq/6: size mismatch");
    }
    if (!(m__unnamed6 == std::string("\x54\x45\x4D\x50\x4F\x3D", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x54\x45\x4D\x50\x4F\x3D", 6), m__unnamed6, m__io, std::string("/types/sequencer/seq/6"));
    }
    if (m_tempo_source.size() != static_cast<std::string::size_type>(3)) {
        throw std::runtime_error("/types/sequencer/seq/7: size mismatch");
    }
    m__dirty = false;
}

mpc2000xl_mid_t::sequencer_t::~sequencer_t() {}

int32_t mpc2000xl_mid_t::denominator() {
    if (f_denominator)
        return m_denominator;
    f_denominator = true;
    m_denominator = 1 << meta_events()->at(4)->meta_event_body()->body().at(1);
    return m_denominator;
}

standard_midi_file_with_running_status_t::meta_event_body_t* mpc2000xl_mid_t::first_meta() {
    if (f_first_meta)
        return m_first_meta;
    f_first_meta = true;
    m_first_meta = meta_events()->at(0)->meta_event_body();
    return m_first_meta;
}

bool mpc2000xl_mid_t::is_mpc2000xl_mid() {
    if (f_is_mpc2000xl_mid)
        return m_is_mpc2000xl_mid;
    f_is_mpc2000xl_mid = true;
    m_is_mpc2000xl_mid =  ((tracks()->size() > 0) && (meta_events()->size() > 0) && (meta_events()->at(0)->event_type() == 240) && (first_meta()->len()->value() == 32) && (kaitai::kstream::bytes_to_str(first_meta()->body(), "ASCII").substr(0, 16 - 0) == std::string("MPC2000XL 1.00  "))) ;
    return m_is_mpc2000xl_mid;
}

std::vector<std::unique_ptr<standard_midi_file_with_running_status_t::track_event_t>>* mpc2000xl_mid_t::meta_events() {
    if (f_meta_events)
        return m_meta_events;
    f_meta_events = true;
    m_meta_events = tracks()->at(0)->events()->event();
    return m_meta_events;
}

uint8_t mpc2000xl_mid_t::numerator() {
    if (f_numerator)
        return m_numerator;
    f_numerator = true;
    m_numerator = meta_events()->at(4)->meta_event_body()->body().at(0);
    return m_numerator;
}

std::string mpc2000xl_mid_t::sequence_name() {
    if (f_sequence_name)
        return m_sequence_name;
    f_sequence_name = true;
    m_sequence_name = kaitai::kstream::bytes_to_str(first_meta()->body(), "ASCII").substr(16, 32 - 16);
    return m_sequence_name;
}

mpc2000xl_mid_t::sequencer_t* mpc2000xl_mid_t::sequencer() {
    if (w_sequencer)
        _write_sequencer();
    if (f_sequencer)
        return m_sequencer.get();
    if (!e_sequencer)
        return nullptr;
    f_sequencer = true;
    kaitai::kstream *io = tracks()->at(0)->events()->_io();
    std::streampos _pos = io->pos();
    io->seek(40);
    m_sequencer = std::unique_ptr<sequencer_t>(new sequencer_t(io, this, m__root));
    m_sequencer->_read();
    io->seek(_pos);
    return m_sequencer.get();
}

void mpc2000xl_mid_t::_write_sequencer() {
    w_sequencer = false;
    kaitai::kstream *io = tracks()->at(0)->events()->_io();
    std::streampos _pos = io->pos();
    io->seek(40);
    if (m_sequencer.get() == nullptr) {
        throw std::runtime_error("/instances/sequencer: nested object is not set");
    }
    m_sequencer.get()->_set_io(io);
    m_sequencer.get()->_write();
    io->seek(_pos);
}

std::string mpc2000xl_mid_t::smpte_offset() {
    if (f_smpte_offset)
        return m_smpte_offset;
    f_smpte_offset = true;
    m_smpte_offset = meta_events()->at(3)->meta_event_body()->body();
    return m_smpte_offset;
}

double mpc2000xl_mid_t::tempo_bpm() {
    if (f_tempo_bpm)
        return m_tempo_bpm;
    f_tempo_bpm = true;
    m_tempo_bpm = 60000000.0 / (((meta_events()->at(2)->meta_event_body()->body().at(0) << 16) + (meta_events()->at(2)->meta_event_body()->body().at(1) << 8)) + meta_events()->at(2)->meta_event_body()->body().at(2));
    return m_tempo_bpm;
}

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "standard_midi_file_with_running_status.h"
#include <cstddef>
#include <stdexcept>
#include "kaitai/exceptions.h"

standard_midi_file_with_running_status_t::standard_midi_file_with_running_status_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::_read() {
    m_hdr = std::unique_ptr<header_t>(new header_t(m__io, this, m__root));
    m_hdr->_read();
    m_tracks = std::unique_ptr<std::vector<std::unique_ptr<track_t>>>(new std::vector<std::unique_ptr<track_t>>());
    const int l_tracks = hdr()->num_tracks();
    for (int i = 0; i < l_tracks; i++) {
        std::unique_ptr<track_t> _t_tracks = std::unique_ptr<track_t>(new track_t(m__io, this, m__root));
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

void standard_midi_file_with_running_status_t::_fetch_instances() {
    m_hdr.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_tracks->size(); ++i) {
        m_tracks->at(i).get()->_fetch_instances();
    }
}

void standard_midi_file_with_running_status_t::_write() {
    if (m_hdr.get() == nullptr) {
        throw std::runtime_error("/seq/0: nested object is not set");
    }
    m_hdr.get()->_set_io(m__io);
    m_hdr.get()->_write();
    if (m_tracks == nullptr) {
        throw std::runtime_error("/seq/1: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<track_t>>::const_iterator it = m_tracks->begin(); it != m_tracks->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::_check() {
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
    for (std::vector<std::unique_ptr<track_t>>::const_iterator it = m_tracks->begin(); it != m_tracks->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

standard_midi_file_with_running_status_t::~standard_midi_file_with_running_status_t() {}

standard_midi_file_with_running_status_t::channel_pressure_event_t::channel_pressure_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::channel_pressure_event_t::_read() {
    m_pressure = m__io->read_u1();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::channel_pressure_event_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::channel_pressure_event_t::_write() {
    m__io->write_u1(m_pressure);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::channel_pressure_event_t::_check() {
    m__dirty = false;
}

standard_midi_file_with_running_status_t::channel_pressure_event_t::~channel_pressure_event_t() {}

standard_midi_file_with_running_status_t::controller_event_t::controller_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::controller_event_t::_read() {
    m_controller = m__io->read_u1();
    m_value = m__io->read_u1();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::controller_event_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::controller_event_t::_write() {
    m__io->write_u1(m_controller);
    m__io->write_u1(m_value);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::controller_event_t::_check() {
    m__dirty = false;
}

standard_midi_file_with_running_status_t::controller_event_t::~controller_event_t() {}

standard_midi_file_with_running_status_t::header_t::header_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::header_t::_read() {
    m_magic = m__io->read_bytes(4);
    if (!(m_magic == std::string("\x4D\x54\x68\x64", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4D\x54\x68\x64", 4), m_magic, m__io, std::string("/types/header/seq/0"));
    }
    m_len_header = m__io->read_u4be();
    m_format = m__io->read_u2be();
    m_num_tracks = m__io->read_u2be();
    m_division = m__io->read_s2be();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::header_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::header_t::_write() {
    m__io->write_bytes(m_magic);
    m__io->write_u4be(m_len_header);
    m__io->write_u2be(m_format);
    m__io->write_u2be(m_num_tracks);
    m__io->write_s2be(m_division);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::header_t::_check() {
    if (m_magic.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/types/header/seq/0: size mismatch");
    }
    if (!(m_magic == std::string("\x4D\x54\x68\x64", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4D\x54\x68\x64", 4), m_magic, m__io, std::string("/types/header/seq/0"));
    }
    m__dirty = false;
}

standard_midi_file_with_running_status_t::header_t::~header_t() {}
const std::set<standard_midi_file_with_running_status_t::meta_event_body_t::meta_type_enum_t> standard_midi_file_with_running_status_t::meta_event_body_t::_values_meta_type_enum_t{
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_SEQUENCE_NUMBER,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_TEXT_EVENT,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_COPYRIGHT,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_SEQUENCE_TRACK_NAME,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_INSTRUMENT_NAME,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_LYRIC_TEXT,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_MARKER_TEXT,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_CUE_POINT,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_MIDI_CHANNEL_PREFIX_ASSIGNMENT,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_END_OF_TRACK,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_TEMPO,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_SMPTE_OFFSET,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_TIME_SIGNATURE,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_KEY_SIGNATURE,
    standard_midi_file_with_running_status_t::meta_event_body_t::META_TYPE_ENUM_SEQUENCER_SPECIFIC_EVENT,
};
bool standard_midi_file_with_running_status_t::meta_event_body_t::_is_defined_meta_type_enum_t(standard_midi_file_with_running_status_t::meta_event_body_t::meta_type_enum_t v) {
    return standard_midi_file_with_running_status_t::meta_event_body_t::_values_meta_type_enum_t.find(v) != standard_midi_file_with_running_status_t::meta_event_body_t::_values_meta_type_enum_t.end();
}

standard_midi_file_with_running_status_t::meta_event_body_t::meta_event_body_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::meta_event_body_t::_read() {
    m_meta_type = static_cast<standard_midi_file_with_running_status_t::meta_event_body_t::meta_type_enum_t>(m__io->read_u1());
    m_len = std::unique_ptr<vlq_base128_be_t>(new vlq_base128_be_t(m__io));
    m_len->_read();
    m_body = m__io->read_bytes(len()->value());
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::meta_event_body_t::_fetch_instances() {
    m_len.get()->_fetch_instances();
}

void standard_midi_file_with_running_status_t::meta_event_body_t::_write() {
    m__io->write_u1(static_cast<uint8_t>(m_meta_type));
    if (m_len.get() == nullptr) {
        throw std::runtime_error("/types/meta_event_body/seq/1: nested object is not set");
    }
    m_len.get()->_set_io(m__io);
    m_len.get()->_write();
    m__io->write_bytes(m_body);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::meta_event_body_t::_check() {
    if (m_len.get() == nullptr) {
        throw std::runtime_error("/types/meta_event_body/seq/1: nested object is not set");
    }
    m_len.get()->_set_io(m__io);
    m_len.get()->_check();
    if (m_body.size() != static_cast<std::string::size_type>(len()->value())) {
        throw std::runtime_error("/types/meta_event_body/seq/2: size mismatch");
    }
    m__dirty = false;
}

standard_midi_file_with_running_status_t::meta_event_body_t::~meta_event_body_t() {}

standard_midi_file_with_running_status_t::note_off_event_t::note_off_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::note_off_event_t::_read() {
    m_note = m__io->read_u1();
    m_velocity = m__io->read_u1();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::note_off_event_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::note_off_event_t::_write() {
    m__io->write_u1(m_note);
    m__io->write_u1(m_velocity);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::note_off_event_t::_check() {
    m__dirty = false;
}

standard_midi_file_with_running_status_t::note_off_event_t::~note_off_event_t() {}

standard_midi_file_with_running_status_t::note_on_event_t::note_on_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::note_on_event_t::_read() {
    m_note = m__io->read_u1();
    m_velocity = m__io->read_u1();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::note_on_event_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::note_on_event_t::_write() {
    m__io->write_u1(m_note);
    m__io->write_u1(m_velocity);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::note_on_event_t::_check() {
    m__dirty = false;
}

standard_midi_file_with_running_status_t::note_on_event_t::~note_on_event_t() {}

standard_midi_file_with_running_status_t::pitch_bend_event_t::pitch_bend_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_adj_bend_value = false;
    f_bend_value = false;
}

void standard_midi_file_with_running_status_t::pitch_bend_event_t::_read() {
    m_b1 = m__io->read_u1();
    m_b2 = m__io->read_u1();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::pitch_bend_event_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::pitch_bend_event_t::_write() {
    m__io->write_u1(m_b1);
    m__io->write_u1(m_b2);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::pitch_bend_event_t::_check() {
    m__dirty = false;
}

standard_midi_file_with_running_status_t::pitch_bend_event_t::~pitch_bend_event_t() {}

int32_t standard_midi_file_with_running_status_t::pitch_bend_event_t::adj_bend_value() {
    if (f_adj_bend_value)
        return m_adj_bend_value;
    f_adj_bend_value = true;
    m_adj_bend_value = bend_value() - 16384;
    return m_adj_bend_value;
}

int32_t standard_midi_file_with_running_status_t::pitch_bend_event_t::bend_value() {
    if (f_bend_value)
        return m_bend_value;
    f_bend_value = true;
    m_bend_value = ((b2() << 7) + b1()) - 16384;
    return m_bend_value;
}

standard_midi_file_with_running_status_t::polyphonic_pressure_event_t::polyphonic_pressure_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::polyphonic_pressure_event_t::_read() {
    m_note = m__io->read_u1();
    m_pressure = m__io->read_u1();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::polyphonic_pressure_event_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::polyphonic_pressure_event_t::_write() {
    m__io->write_u1(m_note);
    m__io->write_u1(m_pressure);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::polyphonic_pressure_event_t::_check() {
    m__dirty = false;
}

standard_midi_file_with_running_status_t::polyphonic_pressure_event_t::~polyphonic_pressure_event_t() {}

standard_midi_file_with_running_status_t::program_change_event_t::program_change_event_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::program_change_event_t::_read() {
    m_program = m__io->read_u1();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::program_change_event_t::_fetch_instances() {
}

void standard_midi_file_with_running_status_t::program_change_event_t::_write() {
    m__io->write_u1(m_program);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::program_change_event_t::_check() {
    m__dirty = false;
}

standard_midi_file_with_running_status_t::program_change_event_t::~program_change_event_t() {}

standard_midi_file_with_running_status_t::sysex_event_body_t::sysex_event_body_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_event_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::sysex_event_body_t::_read() {
    m_len = std::unique_ptr<vlq_base128_be_t>(new vlq_base128_be_t(m__io));
    m_len->_read();
    m_data = m__io->read_bytes(len()->value());
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::sysex_event_body_t::_fetch_instances() {
    m_len.get()->_fetch_instances();
}

void standard_midi_file_with_running_status_t::sysex_event_body_t::_write() {
    if (m_len.get() == nullptr) {
        throw std::runtime_error("/types/sysex_event_body/seq/0: nested object is not set");
    }
    m_len.get()->_set_io(m__io);
    m_len.get()->_write();
    m__io->write_bytes(m_data);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::sysex_event_body_t::_check() {
    if (m_len.get() == nullptr) {
        throw std::runtime_error("/types/sysex_event_body/seq/0: nested object is not set");
    }
    m_len.get()->_set_io(m__io);
    m_len.get()->_check();
    if (m_data.size() != static_cast<std::string::size_type>(len()->value())) {
        throw std::runtime_error("/types/sysex_event_body/seq/1: size mismatch");
    }
    m__dirty = false;
}

standard_midi_file_with_running_status_t::sysex_event_body_t::~sysex_event_body_t() {}

standard_midi_file_with_running_status_t::track_t::track_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::track_t::_read() {
    m_magic = m__io->read_bytes(4);
    if (!(m_magic == std::string("\x4D\x54\x72\x6B", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4D\x54\x72\x6B", 4), m_magic, m__io, std::string("/types/track/seq/0"));
    }
    m_len_events = m__io->read_u4be();
    m__raw_events = m__io->read_bytes(len_events());
    m__io__raw_events = std::unique_ptr<kaitai::kstream>(new kaitai::kstream(m__raw_events));
    m_events = std::unique_ptr<track_events_t>(new track_events_t(m__io__raw_events.get(), this, m__root));
    m_events->_read();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::track_t::_fetch_instances() {
    m_events.get()->_fetch_instances();
}

void standard_midi_file_with_running_status_t::track_t::_write() {
    m__io->write_bytes(m_magic);
    m__io->write_u4be(m_len_events);
    if (m_events.get() == nullptr) {
        throw std::runtime_error("/types/track/seq/2: nested object is not set");
    }
    m__raw_events = std::string(static_cast<std::string::size_type>(len_events()), '\0');
    m__io__raw_events = std::unique_ptr<kaitai::kstream>(new kaitai::kstream(m__raw_events));
    m_events.get()->_set_io(m__io__raw_events.get());
    m_events.get()->_write();
    if (m__io__raw_events.get()->pos() != static_cast<uint64_t>(len_events())) {
        throw std::runtime_error("/types/track/seq/2: serialized size mismatch");
    }
    if (m__io__raw_events.get()->to_byte_array().size() != static_cast<std::string::size_type>(len_events())) {
        throw std::runtime_error("/types/track/seq/2: raw buffer size mismatch");
    }
    m__raw_events = m__io__raw_events.get()->to_byte_array();
    m__io->write_bytes(m__raw_events);
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::track_t::_check() {
    if (m_magic.size() != static_cast<std::string::size_type>(4)) {
        throw std::runtime_error("/types/track/seq/0: size mismatch");
    }
    if (!(m_magic == std::string("\x4D\x54\x72\x6B", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4D\x54\x72\x6B", 4), m_magic, m__io, std::string("/types/track/seq/0"));
    }
    if (m_events.get() == nullptr) {
        throw std::runtime_error("/types/track/seq/2: nested object is not set");
    }
    m__raw_events = std::string(static_cast<std::string::size_type>(len_events()), '\0');
    m__io__raw_events = std::unique_ptr<kaitai::kstream>(new kaitai::kstream(m__raw_events));
    m_events.get()->_set_io(m__io__raw_events.get());
    m_events.get()->_check();
    m__dirty = false;
}

standard_midi_file_with_running_status_t::track_t::~track_t() {}

standard_midi_file_with_running_status_t::track_event_t::track_event_t(uint8_t p_previous_event_type, kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_events_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_previous_event_type = p_previous_event_type;
    m__dirty = false;
    n_event_header = true;
    f_channel = false;
    f_event_type = false;
    f_status_byte_lookahead = false;
    f_using_running_status = false;
    w_status_byte_lookahead = false;
    e_status_byte_lookahead = m__io != nullptr;
}

void standard_midi_file_with_running_status_t::track_event_t::_read() {
    m_v_time = std::unique_ptr<vlq_base128_be_t>(new vlq_base128_be_t(m__io));
    m_v_time->_read();
    n_event_header = true;
    if (!(using_running_status())) {
        n_event_header = false;
        m_event_header = m__io->read_u1();
    }
    if (event_header() == 255) {
        m_meta_event_body = std::unique_ptr<meta_event_body_t>(new meta_event_body_t(m__io, this, m__root));
        m_meta_event_body->_read();
    }
    if (event_header() == 240) {
        m_sysex_body = std::unique_ptr<sysex_event_body_t>(new sysex_event_body_t(m__io, this, m__root));
        m_sysex_body->_read();
    }
    switch (event_type()) {
    case 128: {
        m_event_body = std::unique_ptr<note_off_event_t>(new note_off_event_t(m__io, this, m__root));
        static_cast<note_off_event_t*>(m_event_body.get())->_read();
        break;
    }
    case 144: {
        m_event_body = std::unique_ptr<note_on_event_t>(new note_on_event_t(m__io, this, m__root));
        static_cast<note_on_event_t*>(m_event_body.get())->_read();
        break;
    }
    case 160: {
        m_event_body = std::unique_ptr<polyphonic_pressure_event_t>(new polyphonic_pressure_event_t(m__io, this, m__root));
        static_cast<polyphonic_pressure_event_t*>(m_event_body.get())->_read();
        break;
    }
    case 176: {
        m_event_body = std::unique_ptr<controller_event_t>(new controller_event_t(m__io, this, m__root));
        static_cast<controller_event_t*>(m_event_body.get())->_read();
        break;
    }
    case 192: {
        m_event_body = std::unique_ptr<program_change_event_t>(new program_change_event_t(m__io, this, m__root));
        static_cast<program_change_event_t*>(m_event_body.get())->_read();
        break;
    }
    case 208: {
        m_event_body = std::unique_ptr<channel_pressure_event_t>(new channel_pressure_event_t(m__io, this, m__root));
        static_cast<channel_pressure_event_t*>(m_event_body.get())->_read();
        break;
    }
    case 224: {
        m_event_body = std::unique_ptr<pitch_bend_event_t>(new pitch_bend_event_t(m__io, this, m__root));
        static_cast<pitch_bend_event_t*>(m_event_body.get())->_read();
        break;
    }
    }
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::track_event_t::_fetch_instances() {
    m_v_time.get()->_fetch_instances();
    n_event_header = true;
    if (!(using_running_status())) {
        n_event_header = false;
    }
    if (event_header() == 255) {
        m_meta_event_body.get()->_fetch_instances();
    }
    if (event_header() == 240) {
        m_sysex_body.get()->_fetch_instances();
    }
    switch (event_type()) {
    case 128: {
        {
            note_off_event_t* _switch_obj = dynamic_cast<note_off_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 144: {
        {
            note_on_event_t* _switch_obj = dynamic_cast<note_on_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 160: {
        {
            polyphonic_pressure_event_t* _switch_obj = dynamic_cast<polyphonic_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 176: {
        {
            controller_event_t* _switch_obj = dynamic_cast<controller_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 192: {
        {
            program_change_event_t* _switch_obj = dynamic_cast<program_change_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 208: {
        {
            channel_pressure_event_t* _switch_obj = dynamic_cast<channel_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    case 224: {
        {
            pitch_bend_event_t* _switch_obj = dynamic_cast<pitch_bend_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("switch object type mismatch in _fetch_instances");
            }
            _switch_obj->_fetch_instances();
        }
        break;
    }
    }
    status_byte_lookahead();
    if (f_status_byte_lookahead) {
    }
}

void standard_midi_file_with_running_status_t::track_event_t::_write() {
    w_status_byte_lookahead = false;
    if (m_v_time.get() == nullptr) {
        throw std::runtime_error("/types/track_event/seq/0: nested object is not set");
    }
    m_v_time.get()->_set_io(m__io);
    m_v_time.get()->_write();
    if (!(using_running_status())) {
        m__io->write_u1(m_event_header);
    }
    if (event_header() == 255) {
        if (m_meta_event_body.get() == nullptr) {
            throw std::runtime_error("/types/track_event/seq/2: nested object is not set");
        }
        m_meta_event_body.get()->_set_io(m__io);
        m_meta_event_body.get()->_write();
    }
    if (event_header() == 240) {
        if (m_sysex_body.get() == nullptr) {
            throw std::runtime_error("/types/track_event/seq/3: nested object is not set");
        }
        m_sysex_body.get()->_set_io(m__io);
        m_sysex_body.get()->_write();
    }
    switch (event_type()) {
    case 128: {
        {
            note_off_event_t* _switch_obj = dynamic_cast<note_off_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    case 144: {
        {
            note_on_event_t* _switch_obj = dynamic_cast<note_on_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    case 160: {
        {
            polyphonic_pressure_event_t* _switch_obj = dynamic_cast<polyphonic_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    case 176: {
        {
            controller_event_t* _switch_obj = dynamic_cast<controller_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    case 192: {
        {
            program_change_event_t* _switch_obj = dynamic_cast<program_change_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    case 208: {
        {
            channel_pressure_event_t* _switch_obj = dynamic_cast<channel_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_write();
        }
        break;
    }
    case 224: {
        {
            pitch_bend_event_t* _switch_obj = dynamic_cast<pitch_bend_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
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

void standard_midi_file_with_running_status_t::track_event_t::_check() {
    if (m_v_time.get() == nullptr) {
        throw std::runtime_error("/types/track_event/seq/0: nested object is not set");
    }
    m_v_time.get()->_set_io(m__io);
    m_v_time.get()->_check();
    if (!(using_running_status())) {
        if (!(!n_event_header)) {
            throw std::runtime_error("/types/track_event/seq/1: conditional field is not set");
        }
    } else {
        if (!n_event_header) {
            throw std::runtime_error("/types/track_event/seq/1: conditional field should be absent");
        }
    }
    if (event_header() == 255) {
        if (!(m_meta_event_body.get() != nullptr)) {
            throw std::runtime_error("/types/track_event/seq/2: conditional field is not set");
        }
        if (m_meta_event_body.get() == nullptr) {
            throw std::runtime_error("/types/track_event/seq/2: nested object is not set");
        }
        m_meta_event_body.get()->_set_io(m__io);
        m_meta_event_body.get()->_check();
    } else {
        if (m_meta_event_body.get() != nullptr) {
            throw std::runtime_error("/types/track_event/seq/2: conditional field should be absent");
        }
    }
    if (event_header() == 240) {
        if (!(m_sysex_body.get() != nullptr)) {
            throw std::runtime_error("/types/track_event/seq/3: conditional field is not set");
        }
        if (m_sysex_body.get() == nullptr) {
            throw std::runtime_error("/types/track_event/seq/3: nested object is not set");
        }
        m_sysex_body.get()->_set_io(m__io);
        m_sysex_body.get()->_check();
    } else {
        if (m_sysex_body.get() != nullptr) {
            throw std::runtime_error("/types/track_event/seq/3: conditional field should be absent");
        }
    }
    switch (event_type()) {
    case 128: {
        {
            note_off_event_t* _switch_obj = dynamic_cast<note_off_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    case 144: {
        {
            note_on_event_t* _switch_obj = dynamic_cast<note_on_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    case 160: {
        {
            polyphonic_pressure_event_t* _switch_obj = dynamic_cast<polyphonic_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    case 176: {
        {
            controller_event_t* _switch_obj = dynamic_cast<controller_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    case 192: {
        {
            program_change_event_t* _switch_obj = dynamic_cast<program_change_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    case 208: {
        {
            channel_pressure_event_t* _switch_obj = dynamic_cast<channel_pressure_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    case 224: {
        {
            pitch_bend_event_t* _switch_obj = dynamic_cast<pitch_bend_event_t*>(m_event_body.get());
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: switch object type mismatch");
            }
            if (_switch_obj == nullptr) {
                throw std::runtime_error("/types/track_event/seq/4: nested object is not set");
            }
            _switch_obj->_set_io(m__io);
            _switch_obj->_check();
        }
        break;
    }
    }
    if (e_status_byte_lookahead) {
    }
    m__dirty = false;
}

standard_midi_file_with_running_status_t::track_event_t::~track_event_t() {}

int32_t standard_midi_file_with_running_status_t::track_event_t::channel() {
    if (f_channel)
        return m_channel;
    f_channel = true;
    n_channel = true;
    if (event_type() != 240) {
        n_channel = false;
        m_channel = event_header() & 15;
    }
    return m_channel;
}

int32_t standard_midi_file_with_running_status_t::track_event_t::event_type() {
    if (f_event_type)
        return m_event_type;
    f_event_type = true;
    m_event_type = ((using_running_status()) ? (previous_event_type()) : (event_header() & 240));
    return m_event_type;
}

uint8_t standard_midi_file_with_running_status_t::track_event_t::status_byte_lookahead() {
    if (w_status_byte_lookahead)
        _write_status_byte_lookahead();
    if (f_status_byte_lookahead)
        return m_status_byte_lookahead;
    if (!e_status_byte_lookahead)
        return 0;
    f_status_byte_lookahead = true;
    std::streampos _pos = m__io->pos();
    m__io->seek(_io()->pos());
    m_status_byte_lookahead = m__io->read_u1();
    m__io->seek(_pos);
    return m_status_byte_lookahead;
}

void standard_midi_file_with_running_status_t::track_event_t::_write_status_byte_lookahead() {
    w_status_byte_lookahead = false;
    std::streampos _pos = m__io->pos();
    m__io->seek(_io()->pos());
    m__io->write_u1(m_status_byte_lookahead);
    m__io->seek(_pos);
}

bool standard_midi_file_with_running_status_t::track_event_t::using_running_status() {
    if (f_using_running_status)
        return m_using_running_status;
    f_using_running_status = true;
    m_using_running_status = (status_byte_lookahead() & 128) == 0;
    return m_using_running_status;
}

standard_midi_file_with_running_status_t::track_events_t::track_events_t(kaitai::kstream* p__io, standard_midi_file_with_running_status_t::track_t* p__parent, standard_midi_file_with_running_status_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::track_events_t::_read() {
    m_event = std::unique_ptr<std::vector<std::unique_ptr<track_event_t>>>(new std::vector<std::unique_ptr<track_event_t>>());
    {
        int i = 0;
        while (!m__io->is_eof()) {
            std::unique_ptr<track_event_t> _t_event = std::unique_ptr<track_event_t>(new track_event_t(((i != 0) ? (event()->at(i - 1)->event_type()) : (255)), m__io, this, m__root));
            try {
                _t_event->_read();
            } catch(...) {
                m_event->push_back(std::move(_t_event));
                throw;
            }
            m_event->push_back(std::move(_t_event));
            i++;
        }
    }
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::track_events_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_event->size(); ++i) {
        m_event->at(i).get()->_fetch_instances();
    }
}

void standard_midi_file_with_running_status_t::track_events_t::_write() {
    if (m_event == nullptr) {
        throw std::runtime_error("/types/track_events/seq/0: repeated field is not set");
    }
    if (m_event == nullptr) {
        throw std::runtime_error("/types/track_events/seq/0: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<track_event_t>>::const_iterator it = m_event->begin(); it != m_event->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/track_events/seq/0: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void standard_midi_file_with_running_status_t::track_events_t::_check() {
    if (m_event == nullptr) {
        throw std::runtime_error("/types/track_events/seq/0: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<track_event_t>>::const_iterator it = m_event->begin(); it != m_event->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_event->begin());
        const track_event_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/track_events/seq/0: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

standard_midi_file_with_running_status_t::track_events_t::~track_events_t() {}

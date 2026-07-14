// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_seq_body.h"
#include <cstddef>
#include <stdexcept>
#include "kaitai/exceptions.h"
const std::set<mpc60_seq_body_t::off_on_t> mpc60_seq_body_t::_values_off_on_t{
    mpc60_seq_body_t::OFF_ON_FALSE,
    mpc60_seq_body_t::OFF_ON_TRUE,
};
bool mpc60_seq_body_t::_is_defined_off_on_t(mpc60_seq_body_t::off_on_t v) {
    return mpc60_seq_body_t::_values_off_on_t.find(v) != mpc60_seq_body_t::_values_off_on_t.end();
}

mpc60_seq_body_t::mpc60_seq_body_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc60_seq_body_t::_read() {
    m__dirty = false;
}

void mpc60_seq_body_t::_fetch_instances() {
}

void mpc60_seq_body_t::_write() {
    _fetch_instances();
    m__dirty = false;
}

void mpc60_seq_body_t::_check() {
    m__dirty = false;
}

mpc60_seq_body_t::~mpc60_seq_body_t() {}

mpc60_seq_body_t::sequence_t::sequence_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_events_start = false;
}

void mpc60_seq_body_t::sequence_t::_read() {
    m_sequence_header = std::unique_ptr<sequence_header_t>(new sequence_header_t(m__io, this, m__root));
    m_sequence_header->_read();
    m_track_headers = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>());
    const int l_track_headers = sequence_header()->num_track_headers();
    for (int i = 0; i < l_track_headers; i++) {
        std::unique_ptr<mpc3000_seq_v3_t::track_header_t> _t_track_headers = std::unique_ptr<mpc3000_seq_v3_t::track_header_t>(new mpc3000_seq_v3_t::track_header_t(m__io));
        try {
            _t_track_headers->_read();
        } catch(...) {
            m_track_headers->push_back(std::move(_t_track_headers));
            throw;
        }
        m_track_headers->push_back(std::move(_t_track_headers));
    }
    m_tempo_changes = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>());
    const int l_tempo_changes = sequence_header()->num_tempo_changes();
    for (int i = 0; i < l_tempo_changes; i++) {
        std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t> _t_tempo_changes = std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>(new mpc3000_seq_v3_t::tempo_change_t(m__io));
        try {
            _t_tempo_changes->_read();
        } catch(...) {
            m_tempo_changes->push_back(std::move(_t_tempo_changes));
            throw;
        }
        m_tempo_changes->push_back(std::move(_t_tempo_changes));
    }
    m_events = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>());
    {
        int i = 0;
        mpc3000_seq_v3_t::event_t* _;
        do {
            std::unique_ptr<mpc3000_seq_v3_t::event_t> _t_events = std::unique_ptr<mpc3000_seq_v3_t::event_t>(new mpc3000_seq_v3_t::event_t(i == 0, ((i > 0) ? (events()->at(i - 1)->next_status()) : (255)), sequence_header()->event_stream_length_in_bytes()->value() - (_io()->pos() - events_start()), m__io));
            try {
                _t_events->_read();
            } catch(...) {
                _ = _t_events.get();
                m_events->push_back(std::move(_t_events));
                throw;
            }
            _ = _t_events.get();
            m_events->push_back(std::move(_t_events));
            i++;
        } while (!(sequence_header()->event_stream_length_in_bytes()->value() - (_io()->pos() - events_start()) == 0));
    }
    m__dirty = false;
}

void mpc60_seq_body_t::sequence_t::_fetch_instances() {
    m_sequence_header.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_track_headers->size(); ++i) {
        m_track_headers->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_tempo_changes->size(); ++i) {
        m_tempo_changes->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_events->size(); ++i) {
        m_events->at(i).get()->_fetch_instances();
    }
}

void mpc60_seq_body_t::sequence_t::_write() {
    if (m_sequence_header.get() == nullptr) {
        throw std::runtime_error("/types/sequence/seq/0: nested object is not set");
    }
    m_sequence_header.get()->_set_io(m__io);
    m_sequence_header.get()->_write();
    if (m_track_headers == nullptr) {
        throw std::runtime_error("/types/sequence/seq/1: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>::const_iterator it = m_track_headers->begin(); it != m_track_headers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_tempo_changes == nullptr) {
        throw std::runtime_error("/types/sequence/seq/2: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>::const_iterator it = m_tempo_changes->begin(); it != m_tempo_changes->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/2: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_events == nullptr) {
        throw std::runtime_error("/types/sequence/seq/3: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>::const_iterator it = m_events->begin(); it != m_events->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc60_seq_body_t::sequence_t::_check() {
    if (m_sequence_header.get() == nullptr) {
        throw std::runtime_error("/types/sequence/seq/0: nested object is not set");
    }
    m_sequence_header.get()->_set_io(m__io);
    m_sequence_header.get()->_check();
    if (m_track_headers == nullptr) {
        throw std::runtime_error("/types/sequence/seq/1: repeated field is not set");
    }
    if (m_track_headers->size() != static_cast<std::size_t>(sequence_header()->num_track_headers())) {
        throw std::runtime_error("/types/sequence/seq/1: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>::const_iterator it = m_track_headers->begin(); it != m_track_headers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_tempo_changes == nullptr) {
        throw std::runtime_error("/types/sequence/seq/2: repeated field is not set");
    }
    if (m_tempo_changes->size() != static_cast<std::size_t>(sequence_header()->num_tempo_changes())) {
        throw std::runtime_error("/types/sequence/seq/2: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>::const_iterator it = m_tempo_changes->begin(); it != m_tempo_changes->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/2: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_events == nullptr) {
        throw std::runtime_error("/types/sequence/seq/3: repeated field is not set");
    }
    if (m_events->empty()) {
        throw std::runtime_error("/types/sequence/seq/3: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>::const_iterator it = m_events->begin(); it != m_events->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_events->begin());
        const mpc3000_seq_v3_t::event_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_events->size() - 1);
        if ((sequence_header()->event_stream_length_in_bytes()->value() - (_io()->pos() - events_start()) == 0) != _is_last) {
            throw std::runtime_error("/types/sequence/seq/3: repeat-until condition mismatch");
        }
    }
    m__dirty = false;
}

mpc60_seq_body_t::sequence_t::~sequence_t() {}

int32_t mpc60_seq_body_t::sequence_t::events_start() {
    if (f_events_start)
        return m_events_start;
    f_events_start = true;
    m_events_start = _io()->pos();
    return m_events_start;
}

mpc60_seq_body_t::sequence_header_t::sequence_header_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_t* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_seq_body_t::sequence_header_t::_read() {
    m_sequence_number = m__io->read_u1();
    m_event_stream_length_in_bytes = std::unique_ptr<u3le_t>(new u3le_t(m__io, this, m__root));
    m_event_stream_length_in_bytes->_read();
    m_offset_from_bottom_of_sequence_to_sequence_start = std::unique_ptr<u3le_t>(new u3le_t(m__io, this, m__root));
    m_offset_from_bottom_of_sequence_to_sequence_start->_read();
    m_sequence_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m_loop_to_bar = static_cast<mpc60_seq_body_t::off_on_t>(m__io->read_bits_int_le(1));
    m__unnamed5 = m__io->read_bits_int_le(7);
    m_loop_to_bar_number = m__io->read_u2le();
    m_number_of_bars = m__io->read_u2le();
    m_length_in_ticks = m__io->read_u4le();
    m_tempo = m__io->read_u2le();
    m_smpte_offset = std::unique_ptr<smpte_offset_t>(new smpte_offset_t(m__io, this, m__root));
    m_smpte_offset->_read();
    m_stereo_mix = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_stereo_mix = 32;
    for (int i = 0; i < l_stereo_mix; i++) {
        m_stereo_mix->push_back(std::move(m__io->read_u1()));
    }
    m_stereo_pan = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_stereo_pan = 32;
    for (int i = 0; i < l_stereo_pan; i++) {
        m_stereo_pan->push_back(std::move(m__io->read_u1()));
    }
    m_echo_mix = std::unique_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
    const int l_echo_mix = 32;
    for (int i = 0; i < l_echo_mix; i++) {
        m_echo_mix->push_back(std::move(m__io->read_u1()));
    }
    m_drum_tuning = std::unique_ptr<std::vector<uint16_t>>(new std::vector<uint16_t>());
    const int l_drum_tuning = 32;
    for (int i = 0; i < l_drum_tuning; i++) {
        m_drum_tuning->push_back(std::move(m__io->read_u2le()));
    }
    m_last_active_user_track = m__io->read_u1();
    m_num_tempo_changes = m__io->read_u1();
    m_num_track_headers = m__io->read_u1();
    m__dirty = false;
}

void mpc60_seq_body_t::sequence_header_t::_fetch_instances() {
    m_event_stream_length_in_bytes.get()->_fetch_instances();
    m_offset_from_bottom_of_sequence_to_sequence_start.get()->_fetch_instances();
    m_smpte_offset.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_stereo_mix->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_stereo_pan->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_echo_mix->size(); ++i) {
    }
    for (std::size_t i = 0; i < m_drum_tuning->size(); ++i) {
    }
}

void mpc60_seq_body_t::sequence_header_t::_write() {
    m__io->write_u1(m_sequence_number);
    if (m_event_stream_length_in_bytes.get() == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/1: nested object is not set");
    }
    m_event_stream_length_in_bytes.get()->_set_io(m__io);
    m_event_stream_length_in_bytes.get()->_write();
    if (m_offset_from_bottom_of_sequence_to_sequence_start.get() == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/2: nested object is not set");
    }
    m_offset_from_bottom_of_sequence_to_sequence_start.get()->_set_io(m__io);
    m_offset_from_bottom_of_sequence_to_sequence_start.get()->_write();
    m__io->write_bytes(m_sequence_name);
    m__io->write_bits_int_le(1, static_cast<uint64_t>(m_loop_to_bar));
    m__io->write_bits_int_le(7, m__unnamed5);
    m__io->write_u2le(m_loop_to_bar_number);
    m__io->write_u2le(m_number_of_bars);
    m__io->write_u4le(m_length_in_ticks);
    m__io->write_u2le(m_tempo);
    if (m_smpte_offset.get() == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/10: nested object is not set");
    }
    m_smpte_offset.get()->_set_io(m__io);
    m_smpte_offset.get()->_write();
    if (m_stereo_mix == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/11: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_stereo_mix->begin(); it != m_stereo_mix->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_stereo_pan == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/12: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_stereo_pan->begin(); it != m_stereo_pan->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_echo_mix == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/13: repeated field is not set");
    }
    for (std::vector<uint8_t>::const_iterator it = m_echo_mix->begin(); it != m_echo_mix->end(); ++it) {
        m__io->write_u1((*it));
    }
    if (m_drum_tuning == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/14: repeated field is not set");
    }
    for (std::vector<uint16_t>::const_iterator it = m_drum_tuning->begin(); it != m_drum_tuning->end(); ++it) {
        m__io->write_u2le((*it));
    }
    m__io->write_u1(m_last_active_user_track);
    m__io->write_u1(m_num_tempo_changes);
    m__io->write_u1(m_num_track_headers);
    _fetch_instances();
    m__dirty = false;
}

void mpc60_seq_body_t::sequence_header_t::_check() {
    if (m_event_stream_length_in_bytes.get() == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/1: nested object is not set");
    }
    m_event_stream_length_in_bytes.get()->_set_io(m__io);
    m_event_stream_length_in_bytes.get()->_check();
    if (m_offset_from_bottom_of_sequence_to_sequence_start.get() == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/2: nested object is not set");
    }
    m_offset_from_bottom_of_sequence_to_sequence_start.get()->_set_io(m__io);
    m_offset_from_bottom_of_sequence_to_sequence_start.get()->_check();
    if (m_sequence_name.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/sequence_header/seq/3: size mismatch");
    }
    if (m_smpte_offset.get() == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/10: nested object is not set");
    }
    m_smpte_offset.get()->_set_io(m__io);
    m_smpte_offset.get()->_check();
    if (m_stereo_mix == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/11: repeated field is not set");
    }
    if (m_stereo_mix->size() != static_cast<std::size_t>(32)) {
        throw std::runtime_error("/types/sequence_header/seq/11: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_stereo_mix->begin(); it != m_stereo_mix->end(); ++it) {
    }
    if (m_stereo_pan == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/12: repeated field is not set");
    }
    if (m_stereo_pan->size() != static_cast<std::size_t>(32)) {
        throw std::runtime_error("/types/sequence_header/seq/12: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_stereo_pan->begin(); it != m_stereo_pan->end(); ++it) {
    }
    if (m_echo_mix == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/13: repeated field is not set");
    }
    if (m_echo_mix->size() != static_cast<std::size_t>(32)) {
        throw std::runtime_error("/types/sequence_header/seq/13: repeat-expr size mismatch");
    }
    for (std::vector<uint8_t>::const_iterator it = m_echo_mix->begin(); it != m_echo_mix->end(); ++it) {
    }
    if (m_drum_tuning == nullptr) {
        throw std::runtime_error("/types/sequence_header/seq/14: repeated field is not set");
    }
    if (m_drum_tuning->size() != static_cast<std::size_t>(32)) {
        throw std::runtime_error("/types/sequence_header/seq/14: repeat-expr size mismatch");
    }
    for (std::vector<uint16_t>::const_iterator it = m_drum_tuning->begin(); it != m_drum_tuning->end(); ++it) {
    }
    m__dirty = false;
}

mpc60_seq_body_t::sequence_header_t::~sequence_header_t() {}

mpc60_seq_body_t::smpte_offset_t::smpte_offset_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_header_t* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc60_seq_body_t::smpte_offset_t::_read() {
    m_hundredth_frames = m__io->read_u1();
    if (!(m_hundredth_frames >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_hundredth_frames, m__io, std::string("/types/smpte_offset/seq/0"));
    }
    if (!(m_hundredth_frames <= 99)) {
        throw kaitai::validation_greater_than_error<uint8_t>(99, m_hundredth_frames, m__io, std::string("/types/smpte_offset/seq/0"));
    }
    m_frames = m__io->read_u1();
    if (!(m_frames >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_frames, m__io, std::string("/types/smpte_offset/seq/1"));
    }
    if (!(m_frames <= 29)) {
        throw kaitai::validation_greater_than_error<uint8_t>(29, m_frames, m__io, std::string("/types/smpte_offset/seq/1"));
    }
    m_seconds = m__io->read_u1();
    if (!(m_seconds >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_seconds, m__io, std::string("/types/smpte_offset/seq/2"));
    }
    if (!(m_seconds <= 59)) {
        throw kaitai::validation_greater_than_error<uint8_t>(59, m_seconds, m__io, std::string("/types/smpte_offset/seq/2"));
    }
    m_minutes = m__io->read_u1();
    if (!(m_minutes >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_minutes, m__io, std::string("/types/smpte_offset/seq/3"));
    }
    if (!(m_minutes <= 59)) {
        throw kaitai::validation_greater_than_error<uint8_t>(59, m_minutes, m__io, std::string("/types/smpte_offset/seq/3"));
    }
    m_hours = m__io->read_u1();
    if (!(m_hours >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_hours, m__io, std::string("/types/smpte_offset/seq/4"));
    }
    if (!(m_hours <= 23)) {
        throw kaitai::validation_greater_than_error<uint8_t>(23, m_hours, m__io, std::string("/types/smpte_offset/seq/4"));
    }
    m__dirty = false;
}

void mpc60_seq_body_t::smpte_offset_t::_fetch_instances() {
}

void mpc60_seq_body_t::smpte_offset_t::_write() {
    m__io->write_u1(m_hundredth_frames);
    m__io->write_u1(m_frames);
    m__io->write_u1(m_seconds);
    m__io->write_u1(m_minutes);
    m__io->write_u1(m_hours);
    _fetch_instances();
    m__dirty = false;
}

void mpc60_seq_body_t::smpte_offset_t::_check() {
    if (!(m_hundredth_frames >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_hundredth_frames, m__io, std::string("/types/smpte_offset/seq/0"));
    }
    if (!(m_hundredth_frames <= 99)) {
        throw kaitai::validation_greater_than_error<uint8_t>(99, m_hundredth_frames, m__io, std::string("/types/smpte_offset/seq/0"));
    }
    if (!(m_frames >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_frames, m__io, std::string("/types/smpte_offset/seq/1"));
    }
    if (!(m_frames <= 29)) {
        throw kaitai::validation_greater_than_error<uint8_t>(29, m_frames, m__io, std::string("/types/smpte_offset/seq/1"));
    }
    if (!(m_seconds >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_seconds, m__io, std::string("/types/smpte_offset/seq/2"));
    }
    if (!(m_seconds <= 59)) {
        throw kaitai::validation_greater_than_error<uint8_t>(59, m_seconds, m__io, std::string("/types/smpte_offset/seq/2"));
    }
    if (!(m_minutes >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_minutes, m__io, std::string("/types/smpte_offset/seq/3"));
    }
    if (!(m_minutes <= 59)) {
        throw kaitai::validation_greater_than_error<uint8_t>(59, m_minutes, m__io, std::string("/types/smpte_offset/seq/3"));
    }
    if (!(m_hours >= 0)) {
        throw kaitai::validation_less_than_error<uint8_t>(0, m_hours, m__io, std::string("/types/smpte_offset/seq/4"));
    }
    if (!(m_hours <= 23)) {
        throw kaitai::validation_greater_than_error<uint8_t>(23, m_hours, m__io, std::string("/types/smpte_offset/seq/4"));
    }
    m__dirty = false;
}

mpc60_seq_body_t::smpte_offset_t::~smpte_offset_t() {}

mpc60_seq_body_t::u3le_t::u3le_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_header_t* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_value = false;
}

void mpc60_seq_body_t::u3le_t::_read() {
    m_b12 = m__io->read_u2le();
    m_b3 = m__io->read_u1();
    m__dirty = false;
}

void mpc60_seq_body_t::u3le_t::_fetch_instances() {
}

void mpc60_seq_body_t::u3le_t::_write() {
    m__io->write_u2le(m_b12);
    m__io->write_u1(m_b3);
    _fetch_instances();
    m__dirty = false;
}

void mpc60_seq_body_t::u3le_t::_check() {
    m__dirty = false;
}

mpc60_seq_body_t::u3le_t::~u3le_t() {}

int32_t mpc60_seq_body_t::u3le_t::value() {
    if (f_value)
        return m_value;
    f_value = true;
    m_value = b12() | b3() << 16;
    return m_value;
}

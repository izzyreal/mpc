// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc60_seq_body.h"
#include <cstddef>
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
    _read();
}

void mpc60_seq_body_t::_read() {
}

void mpc60_seq_body_t::_fetch_instances() {
}

mpc60_seq_body_t::~mpc60_seq_body_t() {}

mpc60_seq_body_t::sequence_t::sequence_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_events_start = false;
    _read();
}

void mpc60_seq_body_t::sequence_t::_read() {
    m_sequence_header = std::unique_ptr<sequence_header_t>(new sequence_header_t(m__io, this, m__root));
    m_track_headers = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>());
    const int l_track_headers = sequence_header()->number_of_active_track_headers();
    for (int i = 0; i < l_track_headers; i++) {
        m_track_headers->push_back(std::move(std::unique_ptr<mpc3000_seq_v3_t::track_header_t>(new mpc3000_seq_v3_t::track_header_t(m__io))));
    }
    m_tempo_changes = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>());
    const int l_tempo_changes = sequence_header()->number_of_tempo_changes();
    for (int i = 0; i < l_tempo_changes; i++) {
        m_tempo_changes->push_back(std::move(std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>(new mpc3000_seq_v3_t::tempo_change_t(m__io))));
    }
    m_events = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>());
    {
        int i = 0;
        mpc3000_seq_v3_t::event_t* _;
        do {
            _ = new mpc3000_seq_v3_t::event_t(i == 0, ((i > 0) ? (events()->at(i - 1)->next_status()) : (255)), sequence_header()->sequence_length_in_bytes()->value() - (_io()->pos() - events_start()), m__io);
            m_events->push_back(std::move(std::unique_ptr<mpc3000_seq_v3_t::event_t>(_)));
            i++;
        } while (!(sequence_header()->sequence_length_in_bytes()->value() - (_io()->pos() - events_start()) == 0));
    }
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
    _read();
}

void mpc60_seq_body_t::sequence_header_t::_read() {
    m_sequence_number = m__io->read_u1();
    m_sequence_length_in_bytes = std::unique_ptr<u3le_t>(new u3le_t(m__io, this, m__root));
    m_offset_from_bottom_of_sequence_to_sequence_start = std::unique_ptr<u3le_t>(new u3le_t(m__io, this, m__root));
    m_sequence_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(16), "ASCII");
    m_loop_to_bar = static_cast<mpc60_seq_body_t::off_on_t>(m__io->read_bits_int_le(1));
    m__unnamed5 = m__io->read_bits_int_le(7);
    m_loop_to_bar_number = m__io->read_u2le();
    m_number_of_bars = m__io->read_u2le();
    m_length_in_ticks = m__io->read_u4le();
    m_tempo = m__io->read_u2le();
    m_smpte_offset = std::unique_ptr<smpte_offset_t>(new smpte_offset_t(m__io, this, m__root));
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
    m_number_of_tempo_changes = m__io->read_u1();
    m_number_of_active_track_headers = m__io->read_u1();
}

void mpc60_seq_body_t::sequence_header_t::_fetch_instances() {
    m_sequence_length_in_bytes.get()->_fetch_instances();
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

mpc60_seq_body_t::sequence_header_t::~sequence_header_t() {}

mpc60_seq_body_t::smpte_offset_t::smpte_offset_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_header_t* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
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
}

void mpc60_seq_body_t::smpte_offset_t::_fetch_instances() {
}

mpc60_seq_body_t::smpte_offset_t::~smpte_offset_t() {}

mpc60_seq_body_t::u3le_t::u3le_t(kaitai::kstream* p__io, mpc60_seq_body_t::sequence_header_t* p__parent, mpc60_seq_body_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_value = false;
    _read();
}

void mpc60_seq_body_t::u3le_t::_read() {
    m_b12 = m__io->read_u2le();
    m_b3 = m__io->read_u1();
}

void mpc60_seq_body_t::u3le_t::_fetch_instances() {
}

mpc60_seq_body_t::u3le_t::~u3le_t() {}

int32_t mpc60_seq_body_t::u3le_t::value() {
    if (f_value)
        return m_value;
    f_value = true;
    m_value = b12() | b3() << 16;
    return m_value;
}

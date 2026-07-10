// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc3000_all_v3.h"
#include <cstddef>
#include "kaitai/exceptions.h"

mpc3000_all_v3_t::mpc3000_all_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    _read();
}

void mpc3000_all_v3_t::_read() {
    m_all_file_header = std::unique_ptr<all_file_header_t>(new all_file_header_t(m__io, this, m__root));
    m_sequences = std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>>(new std::vector<std::unique_ptr<sequence_t>>());
    {
        int i = 0;
        sequence_t* _;
        do {
            _ = new sequence_t(m__io, this, m__root);
            m_sequences->push_back(std::move(std::unique_ptr<sequence_t>(_)));
            i++;
        } while (!((_io()->pos() - 6) - all_file_header()->total_number_of_bytes_in_all_sequences() == -1));
    }
    m_songs = std::unique_ptr<std::vector<std::unique_ptr<song_t>>>(new std::vector<std::unique_ptr<song_t>>());
    {
        int i = 0;
        song_t* _;
        do {
            _ = new song_t(m__io, this, m__root);
            m_songs->push_back(std::move(std::unique_ptr<song_t>(_)));
            i++;
        } while (!(songs()->at(i - 1)->number_of_steps() == 0));
    }
}

void mpc3000_all_v3_t::_fetch_instances() {
    m_all_file_header.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_sequences->size(); ++i) {
        m_sequences->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_songs->size(); ++i) {
        m_songs->at(i).get()->_fetch_instances();
    }
}

mpc3000_all_v3_t::~mpc3000_all_v3_t() {}

mpc3000_all_v3_t::all_file_header_t::all_file_header_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_all_v3_t::all_file_header_t::_read() {
    m_file_id = m__io->read_bytes(1);
    if (!(m_file_id == std::string("\x04", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x04", 1), m_file_id, m__io, std::string("/types/all_file_header/seq/0"));
    }
    m_file_version = m__io->read_bytes(1);
    if (!(m_file_version == std::string("\x03", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x03", 1), m_file_version, m__io, std::string("/types/all_file_header/seq/1"));
    }
    m_total_number_of_bytes_in_all_sequences = m__io->read_u4le();
}

void mpc3000_all_v3_t::all_file_header_t::_fetch_instances() {
}

mpc3000_all_v3_t::all_file_header_t::~all_file_header_t() {}

mpc3000_all_v3_t::misc_chunks_t::misc_chunks_t(kaitai::kstream* p__io, mpc3000_all_v3_t::sequence_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_position_after_tempo_changes = false;
    _read();
}

void mpc3000_all_v3_t::misc_chunks_t::_read() {
    m_sequence_header = std::unique_ptr<mpc3000_seq_v3_t::sequence_header_t>(new mpc3000_seq_v3_t::sequence_header_t(m__io));
    m_smpte_offset = std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t>(new mpc3000_seq_v3_t::smpte_offset_t(m__io));
    m_mixer = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>());
    const int l_mixer = 64;
    for (int i = 0; i < l_mixer; i++) {
        m_mixer->push_back(std::move(std::unique_ptr<mpc3000_seq_v3_t::mixer_t>(new mpc3000_seq_v3_t::mixer_t(m__io))));
    }
    m__unnamed3 = m__io->read_bytes(2);
    m_delays = std::unique_ptr<mpc3000_seq_v3_t::delays_t>(new mpc3000_seq_v3_t::delays_t(m__io));
    m__unnamed5 = m__io->read_bytes(3);
    m__unnamed6 = m__io->read_bytes(16);
    m_last_active_track = m__io->read_u1();
    m_number_of_tempo_changes = m__io->read_u1();
    m_number_of_active_track_headers = m__io->read_u1();
    m_track_headers = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>());
    const int l_track_headers = number_of_active_track_headers();
    for (int i = 0; i < l_track_headers; i++) {
        m_track_headers->push_back(std::move(std::unique_ptr<mpc3000_seq_v3_t::track_header_t>(new mpc3000_seq_v3_t::track_header_t(m__io))));
    }
    m_tempo_changes = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>());
    const int l_tempo_changes = number_of_tempo_changes();
    for (int i = 0; i < l_tempo_changes; i++) {
        m_tempo_changes->push_back(std::move(std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>(new mpc3000_seq_v3_t::tempo_change_t(m__io))));
    }
}

void mpc3000_all_v3_t::misc_chunks_t::_fetch_instances() {
    m_sequence_header.get()->_fetch_instances();
    m_smpte_offset.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_mixer->size(); ++i) {
        m_mixer->at(i).get()->_fetch_instances();
    }
    m_delays.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_track_headers->size(); ++i) {
        m_track_headers->at(i).get()->_fetch_instances();
    }
    for (std::size_t i = 0; i < m_tempo_changes->size(); ++i) {
        m_tempo_changes->at(i).get()->_fetch_instances();
    }
}

mpc3000_all_v3_t::misc_chunks_t::~misc_chunks_t() {}

int32_t mpc3000_all_v3_t::misc_chunks_t::position_after_tempo_changes() {
    if (f_position_after_tempo_changes)
        return m_position_after_tempo_changes;
    f_position_after_tempo_changes = true;
    m_position_after_tempo_changes = _io()->pos();
    return m_position_after_tempo_changes;
}

mpc3000_all_v3_t::sequence_t::sequence_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_all_v3_t::sequence_t::_read() {
    m_misc_chunks = std::unique_ptr<misc_chunks_t>(new misc_chunks_t(m__io, this, m__root));
    m_events = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>());
    {
        int i = 0;
        mpc3000_seq_v3_t::event_t* _;
        do {
            _ = new mpc3000_seq_v3_t::event_t(i == 0, ((i > 0) ? (events()->at(i - 1)->next_status()) : (255)), misc_chunks()->sequence_header()->sequence_length_in_bytes()->value() - (_io()->pos() - misc_chunks()->position_after_tempo_changes()), m__io);
            m_events->push_back(std::move(std::unique_ptr<mpc3000_seq_v3_t::event_t>(_)));
            i++;
        } while (!(misc_chunks()->sequence_header()->sequence_length_in_bytes()->value() - (_io()->pos() - misc_chunks()->position_after_tempo_changes()) == 0));
    }
}

void mpc3000_all_v3_t::sequence_t::_fetch_instances() {
    m_misc_chunks.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_events->size(); ++i) {
        m_events->at(i).get()->_fetch_instances();
    }
}

mpc3000_all_v3_t::sequence_t::~sequence_t() {}

mpc3000_all_v3_t::song_t::song_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_all_v3_t::song_t::_read() {
    m_number_of_steps = m__io->read_u1();
    if ( ((number_of_steps() != 0) && (number_of_steps() != 255)) ) {
        m_song_body = std::unique_ptr<song_body_t>(new song_body_t(m__io, this, m__root));
    }
}

void mpc3000_all_v3_t::song_t::_fetch_instances() {
    if ( ((number_of_steps() != 0) && (number_of_steps() != 255)) ) {
        m_song_body.get()->_fetch_instances();
    }
}

mpc3000_all_v3_t::song_t::~song_t() {}
const std::set<mpc3000_all_v3_t::song_t::song_body_t::end_status_t> mpc3000_all_v3_t::song_t::song_body_t::_values_end_status_t{
    mpc3000_all_v3_t::song_t::song_body_t::END_STATUS_STOP_AT_END,
    mpc3000_all_v3_t::song_t::song_body_t::END_STATUS_LOOP_TO_A_STEP,
};
bool mpc3000_all_v3_t::song_t::song_body_t::_is_defined_end_status_t(mpc3000_all_v3_t::song_t::song_body_t::end_status_t v) {
    return mpc3000_all_v3_t::song_t::song_body_t::_values_end_status_t.find(v) != mpc3000_all_v3_t::song_t::song_body_t::_values_end_status_t.end();
}

mpc3000_all_v3_t::song_t::song_body_t::song_body_t(kaitai::kstream* p__io, mpc3000_all_v3_t::song_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_all_v3_t::song_t::song_body_t::_read() {
    m_song_number = m__io->read_u1();
    m_end_status = static_cast<mpc3000_all_v3_t::song_t::song_body_t::end_status_t>(m__io->read_u1());
    m_loop_back_step_number = m__io->read_u1();
    m_song_name = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(16), static_cast<char>(32), false), "ASCII");
    m_smpte_offset = std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t>(new mpc3000_seq_v3_t::smpte_offset_t(m__io));
    m_steps = std::unique_ptr<std::vector<std::unique_ptr<step_t>>>(new std::vector<std::unique_ptr<step_t>>());
    const int l_steps = static_cast<mpc3000_all_v3_t::song_t*>(_parent())->number_of_steps();
    for (int i = 0; i < l_steps; i++) {
        m_steps->push_back(std::move(std::unique_ptr<step_t>(new step_t(m__io, this, m__root))));
    }
}

void mpc3000_all_v3_t::song_t::song_body_t::_fetch_instances() {
    m_smpte_offset.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_steps->size(); ++i) {
        m_steps->at(i).get()->_fetch_instances();
    }
}

mpc3000_all_v3_t::song_t::song_body_t::~song_body_t() {}

mpc3000_all_v3_t::song_t::song_body_t::step_t::step_t(kaitai::kstream* p__io, mpc3000_all_v3_t::song_t::song_body_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    _read();
}

void mpc3000_all_v3_t::song_t::song_body_t::step_t::_read() {
    m_sequence_number = m__io->read_u1();
    m_repetition_count = m__io->read_u1();
}

void mpc3000_all_v3_t::song_t::song_body_t::step_t::_fetch_instances() {
}

mpc3000_all_v3_t::song_t::song_body_t::step_t::~step_t() {}

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "mpc3000_all_v3.h"
#include "kaitai/exceptions.h"
#include <cstddef>
#include <stdexcept>

mpc3000_all_v3_t::mpc3000_all_v3_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
}

void mpc3000_all_v3_t::_read() {
    m_all_file_header = std::unique_ptr<all_file_header_t>(new all_file_header_t(m__io, this, m__root));
    m_all_file_header->_read();
    m_sequences = std::unique_ptr<std::vector<std::unique_ptr<sequence_t>>>(new std::vector<std::unique_ptr<sequence_t>>());
    {
        int i = 0;
        sequence_t* _;
        do {
            std::unique_ptr<sequence_t> _t_sequences = std::unique_ptr<sequence_t>(new sequence_t(m__io, this, m__root));
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
        } while (!(_io()->pos() - 6 >= all_file_header()->total_number_of_bytes_in_all_sequences() - 1));
    }
    m_sequences_terminator = m__io->read_bytes(1);
    if (!(m_sequences_terminator == std::string("\xFF", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\xFF", 1), m_sequences_terminator, m__io, std::string("/seq/2"));
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
        } while (!(_->number_of_steps() == 0));
    }
    m__dirty = false;
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

void mpc3000_all_v3_t::_write() {
    if (m_all_file_header.get() == nullptr) {
        throw std::runtime_error("/seq/0: nested object is not set");
    }
    m_all_file_header.get()->_set_io(m__io);
    m_all_file_header.get()->_write();
    if (m_sequences == nullptr) {
        throw std::runtime_error("/seq/1: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<sequence_t>>::const_iterator it = m_sequences->begin(); it != m_sequences->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m_sequences_terminator);
    if (m_songs == nullptr) {
        throw std::runtime_error("/seq/3: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<song_t>>::const_iterator it = m_songs->begin(); it != m_songs->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_all_v3_t::_check() {
    if (m_all_file_header.get() == nullptr) {
        throw std::runtime_error("/seq/0: nested object is not set");
    }
    m_all_file_header.get()->_set_io(m__io);
    m_all_file_header.get()->_check();
    if (m_sequences == nullptr) {
        throw std::runtime_error("/seq/1: repeated field is not set");
    }
    if (m_sequences->empty()) {
        throw std::runtime_error("/seq/1: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<sequence_t>>::const_iterator it = m_sequences->begin(); it != m_sequences->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_sequences->begin());
        const sequence_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_sequences->size() - 1);
        if ((_io()->pos() - 6 >= all_file_header()->total_number_of_bytes_in_all_sequences() - 1) != _is_last) {
            throw std::runtime_error("/seq/1: repeat-until condition mismatch");
        }
    }
    if (m_sequences_terminator.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/seq/2: size mismatch");
    }
    if (!(m_sequences_terminator == std::string("\xFF", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\xFF", 1), m_sequences_terminator, m__io, std::string("/seq/2"));
    }
    if (m_songs == nullptr) {
        throw std::runtime_error("/seq/3: repeated field is not set");
    }
    if (m_songs->empty()) {
        throw std::runtime_error("/seq/3: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<song_t>>::const_iterator it = m_songs->begin(); it != m_songs->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_songs->begin());
        const song_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/3: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_songs->size() - 1);
        if ((_->number_of_steps() == 0) != _is_last) {
            throw std::runtime_error("/seq/3: repeat-until condition mismatch");
        }
    }
    m__dirty = false;
}

mpc3000_all_v3_t::~mpc3000_all_v3_t() {}

mpc3000_all_v3_t::all_file_header_t::all_file_header_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
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
    m__dirty = false;
}

void mpc3000_all_v3_t::all_file_header_t::_fetch_instances() {
}

void mpc3000_all_v3_t::all_file_header_t::_write() {
    m__io->write_bytes(m_file_id);
    m__io->write_bytes(m_file_version);
    m__io->write_u4le(m_total_number_of_bytes_in_all_sequences);
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_all_v3_t::all_file_header_t::_check() {
    if (m_file_id.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/all_file_header/seq/0: size mismatch");
    }
    if (!(m_file_id == std::string("\x04", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x04", 1), m_file_id, m__io, std::string("/types/all_file_header/seq/0"));
    }
    if (m_file_version.size() != static_cast<std::string::size_type>(1)) {
        throw std::runtime_error("/types/all_file_header/seq/1: size mismatch");
    }
    if (!(m_file_version == std::string("\x03", 1))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x03", 1), m_file_version, m__io, std::string("/types/all_file_header/seq/1"));
    }
    m__dirty = false;
}

mpc3000_all_v3_t::all_file_header_t::~all_file_header_t() {}

mpc3000_all_v3_t::misc_chunks_t::misc_chunks_t(kaitai::kstream* p__io, mpc3000_all_v3_t::sequence_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
    f_position_after_tempo_changes = false;
}

void mpc3000_all_v3_t::misc_chunks_t::_read() {
    m_sequence_header = std::unique_ptr<mpc3000_seq_v3_t::sequence_header_t>(new mpc3000_seq_v3_t::sequence_header_t(m__io));
    m_sequence_header->_read();
    m_smpte_offset = std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t>(new mpc3000_seq_v3_t::smpte_offset_t(m__io));
    m_smpte_offset->_read();
    m_mixer = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>());
    const int l_mixer = 64;
    for (int i = 0; i < l_mixer; i++) {
        std::unique_ptr<mpc3000_seq_v3_t::mixer_t> _t_mixer = std::unique_ptr<mpc3000_seq_v3_t::mixer_t>(new mpc3000_seq_v3_t::mixer_t(m__io));
        try {
            _t_mixer->_read();
        } catch(...) {
            m_mixer->push_back(std::move(_t_mixer));
            throw;
        }
        m_mixer->push_back(std::move(_t_mixer));
    }
    m_reserved_after_mixer = m__io->read_bytes(2);
    if (!(m_reserved_after_mixer == std::string("\x00\x00", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00", 2), m_reserved_after_mixer, m__io, std::string("/types/misc_chunks/seq/3"));
    }
    m_delays = std::unique_ptr<mpc3000_seq_v3_t::delays_t>(new mpc3000_seq_v3_t::delays_t(m__io));
    m_delays->_read();
    m_reserved_after_delays = m__io->read_bytes(3);
    if (!(m_reserved_after_delays == std::string("\x00\x00\x00", 3))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00\x00", 3), m_reserved_after_delays, m__io, std::string("/types/misc_chunks/seq/5"));
    }
    m_reserved_before_track_summary = m__io->read_bytes(16);
    if (!(m_reserved_before_track_summary == std::string("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16), m_reserved_before_track_summary, m__io, std::string("/types/misc_chunks/seq/6"));
    }
    m_last_active_track = m__io->read_u1();
    m_num_tempo_changes = m__io->read_u1();
    m_num_track_headers = m__io->read_u1();
    m_track_headers = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>());
    const int l_track_headers = num_track_headers();
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
    const int l_tempo_changes = num_tempo_changes();
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
    m__dirty = false;
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

void mpc3000_all_v3_t::misc_chunks_t::_write() {
    if (m_sequence_header.get() == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/0: nested object is not set");
    }
    m_sequence_header.get()->_set_io(m__io);
    m_sequence_header.get()->_write();
    if (m_smpte_offset.get() == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/1: nested object is not set");
    }
    m_smpte_offset.get()->_set_io(m__io);
    m_smpte_offset.get()->_write();
    if (m_mixer == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/2: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>::const_iterator it = m_mixer->begin(); it != m_mixer->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc_chunks/seq/2: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    m__io->write_bytes(m_reserved_after_mixer);
    if (m_delays.get() == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/4: nested object is not set");
    }
    m_delays.get()->_set_io(m__io);
    m_delays.get()->_write();
    m__io->write_bytes(m_reserved_after_delays);
    m__io->write_bytes(m_reserved_before_track_summary);
    m__io->write_u1(m_last_active_track);
    m__io->write_u1(m_num_tempo_changes);
    m__io->write_u1(m_num_track_headers);
    if (m_track_headers == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/10: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>::const_iterator it = m_track_headers->begin(); it != m_track_headers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc_chunks/seq/10: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    if (m_tempo_changes == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/11: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>::const_iterator it = m_tempo_changes->begin(); it != m_tempo_changes->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc_chunks/seq/11: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_all_v3_t::misc_chunks_t::_check() {
    if (m_sequence_header.get() == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/0: nested object is not set");
    }
    m_sequence_header.get()->_set_io(m__io);
    m_sequence_header.get()->_check();
    if (m_smpte_offset.get() == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/1: nested object is not set");
    }
    m_smpte_offset.get()->_set_io(m__io);
    m_smpte_offset.get()->_check();
    if (m_mixer == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/2: repeated field is not set");
    }
    if (m_mixer->size() != static_cast<std::size_t>(64)) {
        throw std::runtime_error("/types/misc_chunks/seq/2: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::mixer_t>>::const_iterator it = m_mixer->begin(); it != m_mixer->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc_chunks/seq/2: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_reserved_after_mixer.size() != static_cast<std::string::size_type>(2)) {
        throw std::runtime_error("/types/misc_chunks/seq/3: size mismatch");
    }
    if (!(m_reserved_after_mixer == std::string("\x00\x00", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00", 2), m_reserved_after_mixer, m__io, std::string("/types/misc_chunks/seq/3"));
    }
    if (m_delays.get() == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/4: nested object is not set");
    }
    m_delays.get()->_set_io(m__io);
    m_delays.get()->_check();
    if (m_reserved_after_delays.size() != static_cast<std::string::size_type>(3)) {
        throw std::runtime_error("/types/misc_chunks/seq/5: size mismatch");
    }
    if (!(m_reserved_after_delays == std::string("\x00\x00\x00", 3))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00\x00", 3), m_reserved_after_delays, m__io, std::string("/types/misc_chunks/seq/5"));
    }
    if (m_reserved_before_track_summary.size() != static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/misc_chunks/seq/6: size mismatch");
    }
    if (!(m_reserved_before_track_summary == std::string("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16), m_reserved_before_track_summary, m__io, std::string("/types/misc_chunks/seq/6"));
    }
    if (m_track_headers == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/10: repeated field is not set");
    }
    if (m_track_headers->size() != static_cast<std::size_t>(num_track_headers())) {
        throw std::runtime_error("/types/misc_chunks/seq/10: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::track_header_t>>::const_iterator it = m_track_headers->begin(); it != m_track_headers->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc_chunks/seq/10: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    if (m_tempo_changes == nullptr) {
        throw std::runtime_error("/types/misc_chunks/seq/11: repeated field is not set");
    }
    if (m_tempo_changes->size() != static_cast<std::size_t>(num_tempo_changes())) {
        throw std::runtime_error("/types/misc_chunks/seq/11: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::tempo_change_t>>::const_iterator it = m_tempo_changes->begin(); it != m_tempo_changes->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/misc_chunks/seq/11: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
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
    m__dirty = false;
}

void mpc3000_all_v3_t::sequence_t::_read() {
    m_misc_chunks = std::unique_ptr<misc_chunks_t>(new misc_chunks_t(m__io, this, m__root));
    m_misc_chunks->_read();
    m_events = std::unique_ptr<std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>>(new std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>());
    {
        int i = 0;
        mpc3000_seq_v3_t::event_t* _;
        do {
            std::unique_ptr<mpc3000_seq_v3_t::event_t> _t_events = std::unique_ptr<mpc3000_seq_v3_t::event_t>(new mpc3000_seq_v3_t::event_t(i == 0, ((i > 0) ? (events()->at(i - 1)->next_status()) : (255)), misc_chunks()->sequence_header()->event_stream_length_in_bytes()->value() - (_io()->pos() - misc_chunks()->position_after_tempo_changes()), m__io));
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
        } while (!(misc_chunks()->sequence_header()->event_stream_length_in_bytes()->value() - (_io()->pos() - misc_chunks()->position_after_tempo_changes()) == 0));
    }
    m__dirty = false;
}

void mpc3000_all_v3_t::sequence_t::_fetch_instances() {
    m_misc_chunks.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_events->size(); ++i) {
        m_events->at(i).get()->_fetch_instances();
    }
}

void mpc3000_all_v3_t::sequence_t::_write() {
    if (m_misc_chunks.get() == nullptr) {
        throw std::runtime_error("/types/sequence/seq/0: nested object is not set");
    }
    m_misc_chunks.get()->_set_io(m__io);
    m_misc_chunks.get()->_write();
    if (m_events == nullptr) {
        throw std::runtime_error("/types/sequence/seq/1: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>::const_iterator it = m_events->begin(); it != m_events->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_all_v3_t::sequence_t::_check() {
    if (m_misc_chunks.get() == nullptr) {
        throw std::runtime_error("/types/sequence/seq/0: nested object is not set");
    }
    m_misc_chunks.get()->_set_io(m__io);
    m_misc_chunks.get()->_check();
    if (m_events == nullptr) {
        throw std::runtime_error("/types/sequence/seq/1: repeated field is not set");
    }
    if (m_events->empty()) {
        throw std::runtime_error("/types/sequence/seq/1: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<mpc3000_seq_v3_t::event_t>>::const_iterator it = m_events->begin(); it != m_events->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_events->begin());
        const mpc3000_seq_v3_t::event_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/sequence/seq/1: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_events->size() - 1);
        if ((misc_chunks()->sequence_header()->event_stream_length_in_bytes()->value() - (_io()->pos() - misc_chunks()->position_after_tempo_changes()) == 0) != _is_last) {
            throw std::runtime_error("/types/sequence/seq/1: repeat-until condition mismatch");
        }
    }
    m__dirty = false;
}

mpc3000_all_v3_t::sequence_t::~sequence_t() {}

mpc3000_all_v3_t::song_t::song_t(kaitai::kstream* p__io, mpc3000_all_v3_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::_read() {
    m_number_of_steps = m__io->read_u1();
    if (number_of_steps() != 0) {
        m_song_body = std::unique_ptr<song_body_t>(new song_body_t(m__io, this, m__root));
        m_song_body->_read();
    }
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::_fetch_instances() {
    if (number_of_steps() != 0) {
        m_song_body.get()->_fetch_instances();
    }
}

void mpc3000_all_v3_t::song_t::_write() {
    m__io->write_u1(m_number_of_steps);
    if (number_of_steps() != 0) {
        if (m_song_body.get() == nullptr) {
            throw std::runtime_error("/types/song/seq/1: nested object is not set");
        }
        m_song_body.get()->_set_io(m__io);
        m_song_body.get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::_check() {
    if (number_of_steps() != 0) {
        if (!(m_song_body.get() != nullptr)) {
            throw std::runtime_error("/types/song/seq/1: conditional field is not set");
        }
        if (m_song_body.get() == nullptr) {
            throw std::runtime_error("/types/song/seq/1: nested object is not set");
        }
        m_song_body.get()->_set_io(m__io);
        m_song_body.get()->_check();
    } else {
        if (m_song_body.get() != nullptr) {
            throw std::runtime_error("/types/song/seq/1: conditional field should be absent");
        }
    }
    m__dirty = false;
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
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::song_body_t::_read() {
    m_song_number = m__io->read_u1();
    m_end_status = static_cast<mpc3000_all_v3_t::song_t::song_body_t::end_status_t>(m__io->read_u1());
    m_loop_back_step_number = m__io->read_u1();
    m_song_name = kaitai::kstream::bytes_to_str(kaitai::kstream::bytes_terminate(m__io->read_bytes(16), static_cast<char>(32), false), "ASCII");
    m_smpte_offset = std::unique_ptr<mpc3000_seq_v3_t::smpte_offset_t>(new mpc3000_seq_v3_t::smpte_offset_t(m__io));
    m_smpte_offset->_read();
    m_steps = std::unique_ptr<std::vector<std::unique_ptr<step_t>>>(new std::vector<std::unique_ptr<step_t>>());
    const int l_steps = static_cast<mpc3000_all_v3_t::song_t*>(_parent())->number_of_steps();
    for (int i = 0; i < l_steps; i++) {
        std::unique_ptr<step_t> _t_steps = std::unique_ptr<step_t>(new step_t(m__io, this, m__root));
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

void mpc3000_all_v3_t::song_t::song_body_t::_fetch_instances() {
    m_smpte_offset.get()->_fetch_instances();
    for (std::size_t i = 0; i < m_steps->size(); ++i) {
        m_steps->at(i).get()->_fetch_instances();
    }
}

void mpc3000_all_v3_t::song_t::song_body_t::_write() {
    m__io->write_u1(m_song_number);
    m__io->write_u1(static_cast<uint8_t>(m_end_status));
    m__io->write_u1(m_loop_back_step_number);
    std::string _bufm_song_name = m_song_name;
    if (_bufm_song_name.size() < static_cast<std::string::size_type>(16)) {
        _bufm_song_name += std::string(1, static_cast<char>(32));
    }
    if (_bufm_song_name.size() < static_cast<std::string::size_type>(16)) {
        _bufm_song_name.append(static_cast<std::string::size_type>(16) - _bufm_song_name.size(), static_cast<char>(0));
    }
    m__io->write_bytes(_bufm_song_name);
    if (m_smpte_offset.get() == nullptr) {
        throw std::runtime_error("/types/song/types/song_body/seq/4: nested object is not set");
    }
    m_smpte_offset.get()->_set_io(m__io);
    m_smpte_offset.get()->_write();
    if (m_steps == nullptr) {
        throw std::runtime_error("/types/song/types/song_body/seq/5: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<step_t>>::const_iterator it = m_steps->begin(); it != m_steps->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/song/types/song_body/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::song_body_t::_check() {
    if (m_song_name.size() > static_cast<std::string::size_type>(16)) {
        throw std::runtime_error("/types/song/types/song_body/seq/3: size mismatch");
    }
    if (m_song_name.find(static_cast<char>(32)) != std::string::npos) {
        throw std::runtime_error("/types/song/types/song_body/seq/3: terminator must not appear in value");
    }
    if (m_smpte_offset.get() == nullptr) {
        throw std::runtime_error("/types/song/types/song_body/seq/4: nested object is not set");
    }
    m_smpte_offset.get()->_set_io(m__io);
    m_smpte_offset.get()->_check();
    if (m_steps == nullptr) {
        throw std::runtime_error("/types/song/types/song_body/seq/5: repeated field is not set");
    }
    if (m_steps->size() != static_cast<std::size_t>(static_cast<mpc3000_all_v3_t::song_t*>(_parent())->number_of_steps())) {
        throw std::runtime_error("/types/song/types/song_body/seq/5: repeat-expr size mismatch");
    }
    for (std::vector<std::unique_ptr<step_t>>::const_iterator it = m_steps->begin(); it != m_steps->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/types/song/types/song_body/seq/5: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
    }
    m__dirty = false;
}

mpc3000_all_v3_t::song_t::song_body_t::~song_body_t() {}

mpc3000_all_v3_t::song_t::song_body_t::step_t::step_t(kaitai::kstream* p__io, mpc3000_all_v3_t::song_t::song_body_t* p__parent, mpc3000_all_v3_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::song_body_t::step_t::_read() {
    m_sequence_number = m__io->read_u1();
    m_repetition_count = m__io->read_u1();
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::song_body_t::step_t::_fetch_instances() {
}

void mpc3000_all_v3_t::song_t::song_body_t::step_t::_write() {
    m__io->write_u1(m_sequence_number);
    m__io->write_u1(m_repetition_count);
    _fetch_instances();
    m__dirty = false;
}

void mpc3000_all_v3_t::song_t::song_body_t::step_t::_check() {
    m__dirty = false;
}

mpc3000_all_v3_t::song_t::song_body_t::step_t::~step_t() {}

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "vlq_base128_be.h"
#include <cstddef>
#include <stdexcept>

vlq_base128_be_t::vlq_base128_be_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, vlq_base128_be_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;
    m__dirty = false;
    f_last = false;
    f_value = false;
}

void vlq_base128_be_t::_read() {
    m_groups = std::unique_ptr<std::vector<std::unique_ptr<group_t>>>(new std::vector<std::unique_ptr<group_t>>());
    {
        int i = 0;
        group_t* _;
        do {
            std::unique_ptr<group_t> _t_groups = std::unique_ptr<group_t>(new group_t(m__io, this, m__root));
            try {
                _t_groups->_read();
            } catch(...) {
                _ = _t_groups.get();
                m_groups->push_back(std::move(_t_groups));
                throw;
            }
            _ = _t_groups.get();
            m_groups->push_back(std::move(_t_groups));
            i++;
        } while (!(!(_->has_next())));
    }
    m__dirty = false;
}

void vlq_base128_be_t::_fetch_instances() {
    for (std::size_t i = 0; i < m_groups->size(); ++i) {
        m_groups->at(i).get()->_fetch_instances();
    }
}

void vlq_base128_be_t::_write() {
    if (m_groups == nullptr) {
        throw std::runtime_error("/seq/0: repeated field is not set");
    }
    for (std::vector<std::unique_ptr<group_t>>::const_iterator it = m_groups->begin(); it != m_groups->end(); ++it) {
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/0: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_write();
    }
    _fetch_instances();
    m__dirty = false;
}

void vlq_base128_be_t::_check() {
    if (m_groups == nullptr) {
        throw std::runtime_error("/seq/0: repeated field is not set");
    }
    if (m_groups->empty()) {
        throw std::runtime_error("/seq/0: repeat-until field must not be empty");
    }
    for (std::vector<std::unique_ptr<group_t>>::const_iterator it = m_groups->begin(); it != m_groups->end(); ++it) {
        const std::size_t i = static_cast<std::size_t>(it - m_groups->begin());
        const group_t* _ = (*it).get();
        if ((*it).get() == nullptr) {
            throw std::runtime_error("/seq/0: nested object is not set");
        }
        (*it).get()->_set_io(m__io);
        (*it).get()->_check();
        const bool _is_last = (i == m_groups->size() - 1);
        if ((!(_->has_next())) != _is_last) {
            throw std::runtime_error("/seq/0: repeat-until condition mismatch");
        }
    }
    m__dirty = false;
}

vlq_base128_be_t::~vlq_base128_be_t() {}

vlq_base128_be_t::group_t::group_t(kaitai::kstream* p__io, vlq_base128_be_t* p__parent, vlq_base128_be_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__dirty = false;
}

void vlq_base128_be_t::group_t::_read() {
    m_has_next = m__io->read_bits_int_be(1);
    m_value = m__io->read_bits_int_be(7);
    m__dirty = false;
}

void vlq_base128_be_t::group_t::_fetch_instances() {
}

void vlq_base128_be_t::group_t::_write() {
    m__io->write_bits_int_be(1, ((m_has_next) ? 1 : 0));
    m__io->write_bits_int_be(7, m_value);
    _fetch_instances();
    m__dirty = false;
}

void vlq_base128_be_t::group_t::_check() {
    m__dirty = false;
}

vlq_base128_be_t::group_t::~group_t() {}

int32_t vlq_base128_be_t::last() {
    if (f_last)
        return m_last;
    f_last = true;
    m_last = groups()->size() - 1;
    return m_last;
}

uint64_t vlq_base128_be_t::value() {
    if (f_value)
        return m_value;
    f_value = true;
    m_value = static_cast<uint64_t>(((((((groups()->at(last())->value() + ((last() >= 1) ? (groups()->at(last() - 1)->value() << 7) : (0))) + ((last() >= 2) ? (groups()->at(last() - 2)->value() << 14) : (0))) + ((last() >= 3) ? (groups()->at(last() - 3)->value() << 21) : (0))) + ((last() >= 4) ? (groups()->at(last() - 4)->value() << 28) : (0))) + ((last() >= 5) ? (groups()->at(last() - 5)->value() << 35) : (0))) + ((last() >= 6) ? (groups()->at(last() - 6)->value() << 42) : (0))) + ((last() >= 7) ? (groups()->at(last() - 7)->value() << 49) : (0)));
    return m_value;
}

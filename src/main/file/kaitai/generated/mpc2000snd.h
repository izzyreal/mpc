#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc2000snd_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

class mpc2000snd_t : public kaitai::kstruct {

public:

private:
    bool m__dirty;

public:

    mpc2000snd_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc2000snd_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc2000snd_t();
    std::string magic() const { return m_magic; }
    void set_magic(std::string _v) { m__dirty = true; m_magic = std::move(_v); }
    std::string name() const { return m_name; }
    void set_name(std::string _v) { m__dirty = true; m_name = std::move(_v); }
    uint8_t level() const { return m_level; }
    void set_level(uint8_t _v) { m__dirty = true; m_level = std::move(_v); }
    int8_t tune() const { return m_tune; }
    void set_tune(int8_t _v) { m__dirty = true; m_tune = std::move(_v); }
    bool stereo() const { return m_stereo; }
    void set_stereo(bool _v) { m__dirty = true; m_stereo = std::move(_v); }
    uint32_t start() const { return m_start; }
    void set_start(uint32_t _v) { m__dirty = true; m_start = std::move(_v); }
    uint32_t end() const { return m_end; }
    void set_end(uint32_t _v) { m__dirty = true; m_end = std::move(_v); }
    uint32_t frame_count() const { return m_frame_count; }
    void set_frame_count(uint32_t _v) { m__dirty = true; m_frame_count = std::move(_v); }
    uint32_t loop_frame_count() const { return m_loop_frame_count; }
    void set_loop_frame_count(uint32_t _v) { m__dirty = true; m_loop_frame_count = std::move(_v); }
    bool loop_enabled() const { return m_loop_enabled; }
    void set_loop_enabled(bool _v) { m__dirty = true; m_loop_enabled = std::move(_v); }
    uint8_t beat_count() const { return m_beat_count; }
    void set_beat_count(uint8_t _v) { m__dirty = true; m_beat_count = std::move(_v); }
    uint16_t sample_rate() const { return m_sample_rate; }
    void set_sample_rate(uint16_t _v) { m__dirty = true; m_sample_rate = std::move(_v); }
    std::vector<int16_t>* frames() const { return m_frames.get(); }
    void set_frames(std::unique_ptr<std::vector<int16_t>> _v) { m__dirty = true; m_frames = std::move(_v); }
    mpc2000snd_t* _root() const { return m__root; }
    void set__root(mpc2000snd_t* _v) { m__dirty = true; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; m__parent = std::move(_v); }

private:
    std::string m_magic;
    std::string m_name;
    uint8_t m_level;
    int8_t m_tune;
    bool m_stereo;
    uint32_t m_start;
    uint32_t m_end;
    uint32_t m_frame_count;
    uint32_t m_loop_frame_count;
    bool m_loop_enabled;
    uint8_t m_beat_count;
    uint16_t m_sample_rate;
    std::unique_ptr<std::vector<int16_t>> m_frames;
    mpc2000snd_t* m__root;
    kaitai::kstruct* m__parent;
};

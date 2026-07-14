#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class mpc3000_snd_v2_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

/**
 * Provisional parser for MPC3000 SND files whose first two bytes are 0x01 0x02.
 * 
 * This definition is currently based on one hardware MPC3000 sample:
 * SOUND017.SND, recorded as a two-second mono sample. That file is confirmed in
 * the local hardware MPC3000 OS 3.11 corpus:
 * 
 * /Users/izmar/projects/VMPC2000XL/reverse_engineer/mpc3000-os3.11/empty/SOUND017.SND
 * 
 * The known-good interpretation for that file is a 38-byte header followed by
 * signed 16-bit little-endian PCM at 44100 Hz.
 * 
 * Known producers of this 0x01 0x02 family currently include:
 * 
 * - hardware MPC3000 OS 3.11
 * - MAME MPC3000 firmware 3.10
 * 
 * A focused MAME MPC3000 3.10 save-session probe established the following:
 * 
 * - `level` is the saved Vol% byte. Files saved with Vol% 50, 100, and 200
 *   encoded `0x32`, `0x64`, and `0xc8` respectively.
 * - `unknown_2` is not a fixed constant and is not sample-rate metadata. Across
 *   a sequence of saves of the same 88200-frame sample, it increased by exactly
 *   `frame_count` each time: `0x000107d0`, `0x00026058`, `0x0003b8e0`,
 *   `0x00051168`, `0x000669f0`, `0x0007c278`. This strongly suggests a running
 *   allocator / sample-offset style value rather than an intrinsic property of
 *   the sample itself.
 * - In that same save path, editing `Soft st` to `000.500.00` and `Soft end` to
 *   `002.500.00` did not change the persisted payload or the bytes currently
 *   mapped as `start`, `end`, and `frame_count`. So those fields remain
 *   provisional, and the save path appears to flatten or ignore those edit
 *   parameters.
 * 
 * Field names other than file_id, file_version, name, level, and sample_data are
 * still provisional.
 */

class mpc3000_snd_v2_t : public kaitai::kstruct {

public:

private:
    bool m__dirty;

public:

    mpc3000_snd_v2_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc3000_snd_v2_t* p__root = nullptr);
    void _read();
    void _fetch_instances();
    void _write();
    void _check();
    ~mpc3000_snd_v2_t();
    int8_t header_size();
    void _invalidate_header_size() { f_header_size = false; }

    /**
     * Hardware MPC3000 SND probe SOUND017.SND has 88200 frames for a known two-second recording, implying 44100 Hz. This is exposed as a constant until sample-rate encoding is identified in the header.
     */
    int32_t sample_rate();
    void _invalidate_sample_rate() { f_sample_rate = false; }
    std::string file_id() const { return m_file_id; }
    void set_file_id(std::string _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_file_id = std::move(_v); }
    std::string file_version() const { return m_file_version; }
    void set_file_version(std::string _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_file_version = std::move(_v); }
    std::string name() const { return m_name; }
    void set_name(std::string _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_name = std::move(_v); }

    /**
     * Observed as 100 in SOUND017.SND.
     */
    uint8_t level() const { return m_level; }
    void set_level(uint8_t _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_level = std::move(_v); }

    /**
     * Observed as 0x0000 in SOUND017.SND.
     */
    std::string unknown_1() const { return m_unknown_1; }
    void set_unknown_1(std::string _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_unknown_1 = std::move(_v); }

    /**
     * Provisional. Observed as 0 in SOUND017.SND and unchanged in a MAME 3.10 save-session probe even when `Soft st` was edited to `000.500.00`.
     */
    uint32_t start() const { return m_start; }
    void set_start(uint32_t _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_start = std::move(_v); }

    /**
     * Provisional. Observed as 88200 in SOUND017.SND and unchanged in a MAME 3.10 save-session probe even when `Soft end` was edited.
     */
    uint32_t end() const { return m_end; }
    void set_end(uint32_t _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_end = std::move(_v); }

    /**
     * Strongly supported. In SOUND017.SND this is 88200, and 38 + frame_count * 2 equals the file size. It also matched the delta of `unknown_2` across repeated MAME 3.10 saves of the same sample.
     */
    uint32_t frame_count() const { return m_frame_count; }
    void set_frame_count(uint32_t _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_frame_count = std::move(_v); }

    /**
     * Not yet fully classified. In a MAME MPC3000 3.10 probe, this value increased by exactly `frame_count` on each save of the same sample, which strongly suggests a running allocator / sample-offset style value rather than sample-rate metadata.
     */
    uint32_t unknown_2() const { return m_unknown_2; }
    void set_unknown_2(uint32_t _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_unknown_2 = std::move(_v); }
    std::vector<int16_t>* sample_data() const { return m_sample_data.get(); }
    void set_sample_data(std::unique_ptr<std::vector<int16_t>> _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m_sample_data = std::move(_v); }
    mpc3000_snd_v2_t* _root() const { return m__root; }
    void set__root(mpc3000_snd_v2_t* _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m__root = std::move(_v); }
    kaitai::kstruct* _parent() const { return m__parent; }
    void set__parent(kaitai::kstruct* _v) { m__dirty = true; f_header_size = false; f_sample_rate = false; m__parent = std::move(_v); }

private:
    bool f_header_size;
    int8_t m_header_size;
    bool f_sample_rate;
    int32_t m_sample_rate;
    std::string m_file_id;
    std::string m_file_version;
    std::string m_name;
    uint8_t m_level;
    std::string m_unknown_1;
    uint32_t m_start;
    uint32_t m_end;
    uint32_t m_frame_count;
    uint32_t m_unknown_2;
    std::unique_ptr<std::vector<int16_t>> m_sample_data;
    mpc3000_snd_v2_t* m__root;
    kaitai::kstruct* m__parent;
};

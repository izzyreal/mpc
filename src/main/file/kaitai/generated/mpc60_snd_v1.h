#pragma once

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <memory>
#include <vector>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif
namespace mpc {
    namespace file {
        namespace kaitai {
            namespace kaitai = ::kaitai;
            namespace generated {

                /**
                 * Parser for MPC60 SND files whose first two bytes are 0x01 0x01.
                 * 
                 * This definition is based on hardware MPC60 SND files plus a native MAME MPC60
                 * 2.14 ROCK.SET export corpus. The header layout and packed sample byte count
                 * are supported by all currently preserved native MPC60 SND files. The packed
                 * 12-bit sample-code layout matches the MPC60 SET sample storage layout; the
                 * stateful conversion from those codes to imported 16-bit PCM is documented
                 * separately in MPC60_12BIT_SAMPLE_DECODER.md rather than expressed directly in
                 * this schema.
                 * 
                 * Known evidence:
                 * - SOUND002.SND: 6039 bytes total, 4000 samples, 6000 sample bytes
                 * - SOUND003.SND: 120039 bytes total, 80000 samples, 120000 sample bytes
                 * - 17 native MAME MPC60 2.14 ROCK.SET exports preserved in codex-mame
                 * - all imply a 39-byte header and 12-bit packed sample storage
                 * - offsets 0x21..0x25 are zero in all 42 preserved MPC60 SND files checked
                 * 
                 * The MPC60 v3.10 manual describes SND contents as sound name, sample data,
                 * tuning, volume, soft start, and soft end. Native 2.14 SET-to-SND exports also
                 * show that SET-only fields such as pitch factor, attack rate, mix/pan, output,
                 * echo, and sound characteristics are not copied into standalone SND headers.
                 */

                class mpc60_snd_v1_t : public kaitai::kstruct {

                public:
                    class sample_pair_t;

                    mpc60_snd_v1_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = nullptr, mpc60_snd_v1_t* p__root = nullptr);

                private:
                    void _read();

                public:
                    void _fetch_instances();
                    ~mpc60_snd_v1_t();

                    class sample_pair_t : public kaitai::kstruct {

                    public:

                        sample_pair_t(kaitai::kstream* p__io, mpc60_snd_v1_t* p__parent = nullptr, mpc60_snd_v1_t* p__root = nullptr);

                    private:
                        void _read();

                    public:
                        void _fetch_instances();
                        ~sample_pair_t();
                        int32_t sample0_code();
                        int32_t sample1_code();
                        uint8_t byte0() const { return m_byte0; }
                        uint8_t byte1() const { return m_byte1; }
                        uint8_t byte2() const { return m_byte2; }
                        mpc60_snd_v1_t* _root() const { return m__root; }
                        kaitai::kstruct* _parent() const { return m__parent; }

                    private:
                        bool f_sample0_code;
                        int32_t m_sample0_code;
                        bool f_sample1_code;
                        int32_t m_sample1_code;
                        uint8_t m_byte0;
                        uint8_t m_byte1;
                        uint8_t m_byte2;
                        mpc60_snd_v1_t* m__root;
                        mpc60_snd_v1_t* m__parent;
                    };

                public:
                    int8_t header_size();

                    /**
                     * Two 12-bit sample words are packed into three bytes. The expression rounds
                     * odd sample counts up to the next pair, matching common 12-bit packing
                     * practice.
                     */
                    int32_t packed_sample_data_byte_count();
                    int32_t packed_sample_pair_count();

                    /**
                     * Current hardware/file evidence points to 40 kHz for this version-family.
                     * If later MPC60 SND evidence shows multiple rates, promote this into a real
                     * parsed field or a version split.
                     */
                    int32_t sample_rate();
                    std::string file_id() const { return m_file_id; }
                    std::string file_version() const { return m_file_version; }
                    std::string name() const { return m_name; }

                    /**
                     * Number of 12-bit mono sample words. Supported by file size:
                     * header_size + packed_sample_data_byte_count equals total file size for
                     * both known hardware MPC60 SND files.
                     */
                    uint32_t sample_count() const { return m_sample_count; }

                    /**
                     * Playback start time relative to the beginning of sample data, in
                     * milliseconds. This matches the same-named MPC60 SET sound-directory
                     * field for the native 2.14 ROCK.SET exports.
                     */
                    uint16_t time_from_start_of_data_to_start_play_msec() const { return m_time_from_start_of_data_to_start_play_msec; }

                    /**
                     * Playback end time relative to the beginning of sample data, in
                     * milliseconds. This matches the same-named MPC60 SET sound-directory
                     * field for the native 2.14 ROCK.SET exports.
                     * 
                     * Some hardware-created files store the final millisecond index rather than
                     * the nominal duration: SOUND002.SND stores 99 for a 4000-sample / 100 ms
                     * sound, and SOUND003.SND stores 1999 for 80000 samples / 2000 ms.
                     * Native 2.14 SET exports observed so far store the nominal end time.
                     */
                    uint32_t time_from_start_of_data_to_end_of_play_msec() const { return m_time_from_start_of_data_to_end_of_play_msec; }

                    /**
                     * Decay time in milliseconds. This matches the same-named MPC60 SET
                     * sound-directory field for the native 2.14 ROCK.SET exports.
                     */
                    uint16_t decay_time_msec() const { return m_decay_time_msec; }

                    /**
                     * Sound volume percentage from Edit a Sound page 1. Confirmed on MAME
                     * MPC60 2.14 by changing only Volume% from 100 to 99 for SNARE#2 from
                     * ROCK.SET: the saved SND changed only byte offset 0x1f from 0x64 to 0x63
                     * and left packed sample data byte-identical.
                     */
                    uint8_t volume_percent() const { return m_volume_percent; }

                    /**
                     * Sound tuning from Edit a Sound page 1. Confirmed on MAME MPC60 2.14 by
                     * changing only Tuning from 0 to 1 for SNARE#2 from ROCK.SET: the saved SND
                     * changed only byte offset 0x20 from 0x00 to 0x01 and left packed sample
                     * data byte-identical.
                     * 
                     * JAUND017.SND, produced by loading an MPC3000 SND on hardware MPC60 and
                     * resaving it, stores 17 here. That is consistent with this field being
                     * sound tuning / import pitch adjustment, not part of a 16-bit volume value.
                     */
                    int8_t tuning() const { return m_tuning; }

                    /**
                     * Observed as all zeroes in all current MPC60 SND evidence, including 42
                     * preserved hardware and MAME MPC60 SND files.
                     * 
                     * These bytes do not correspond to the richer MPC60 SET sound-directory
                     * fields: native 2.14 ROCK.SET exports vary in pitch factor, attack rate,
                     * mix/pan, output, echo, and sound-characteristics values, while the
                     * corresponding SND exports keep this region zero.
                     */
                    std::string reserved_zeroes() const { return m_reserved_zeroes; }

                    /**
                     * Velocity-to-volume percentage from Edit a Sound page 2,
                     * "Vel>vol(0-100)". Confirmed on MAME MPC60 2.14 by changing only
                     * Vel>vol from 100 to 99 for SNARE#2 from ROCK.SET: the saved SND changed
                     * only byte offset 0x26 from 0x64 to 0x63 and left packed sample data
                     * byte-identical.
                     */
                    uint8_t velocity_to_volume_percent() const { return m_velocity_to_volume_percent; }

                    /**
                     * Packed 12-bit sample code pairs. This is the same little-bit-endian
                     * two-words-in-three-bytes layout used by MPC60 SET sample data and
                     * documented in MPC60_12BIT_SAMPLE_DECODER.md.
                     * 
                     * These are sample codes, not final decoded audio amplitudes. The exact
                     * MPC2000XL import conversion is stateful consumer logic and is intentionally
                     * outside this structural schema.
                     */
                    std::vector<std::unique_ptr<sample_pair_t>>* sample_data_pairs() const { return m_sample_data_pairs.get(); }
                    mpc60_snd_v1_t* _root() const { return m__root; }
                    kaitai::kstruct* _parent() const { return m__parent; }

                private:
                    bool f_header_size;
                    int8_t m_header_size;
                    bool f_packed_sample_data_byte_count;
                    int32_t m_packed_sample_data_byte_count;
                    bool f_packed_sample_pair_count;
                    int32_t m_packed_sample_pair_count;
                    bool f_sample_rate;
                    int32_t m_sample_rate;
                    std::string m_file_id;
                    std::string m_file_version;
                    std::string m_name;
                    uint32_t m_sample_count;
                    uint16_t m_time_from_start_of_data_to_start_play_msec;
                    uint32_t m_time_from_start_of_data_to_end_of_play_msec;
                    uint16_t m_decay_time_msec;
                    uint8_t m_volume_percent;
                    int8_t m_tuning;
                    std::string m_reserved_zeroes;
                    uint8_t m_velocity_to_volume_percent;
                    std::unique_ptr<std::vector<std::unique_ptr<sample_pair_t>>> m_sample_data_pairs;
                    mpc60_snd_v1_t* m__root;
                    kaitai::kstruct* m__parent;
                };
            }
        }
    }
}

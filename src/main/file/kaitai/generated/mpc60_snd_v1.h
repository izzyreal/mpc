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
                 * The file uses a 39-byte header followed by packed 12-bit mono sample codes.
                 * Two codes occupy three bytes, using the same packing layout as MPC60 SET
                 * sample data. Converting those codes to PCM is stateful consumer logic and
                 * intentionally outside this structural schema.
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
                     * This file-format version uses a 40 kHz sample rate.
                     */
                    int32_t sample_rate();
                    std::string file_id() const { return m_file_id; }
                    std::string file_version() const { return m_file_version; }
                    std::string name() const { return m_name; }

                    /**
                     * Number of packed 12-bit mono sample codes.
                     */
                    uint32_t sample_count() const { return m_sample_count; }

                    /**
                     * Playback start time relative to sample data, in milliseconds.
                     */
                    uint16_t time_from_start_of_data_to_start_play_msec() const { return m_time_from_start_of_data_to_start_play_msec; }

                    /**
                     * Playback end time relative to sample data, in milliseconds. Some files
                     * store the final millisecond index rather than a duration.
                     */
                    uint32_t time_from_start_of_data_to_end_of_play_msec() const { return m_time_from_start_of_data_to_end_of_play_msec; }

                    /**
                     * Decay time in milliseconds.
                     */
                    uint16_t decay_time_msec() const { return m_decay_time_msec; }

                    /**
                     * Sound volume percentage.
                     */
                    uint8_t volume_percent() const { return m_volume_percent; }

                    /**
                     * Sound tuning or import pitch adjustment.
                     */
                    int8_t tuning() const { return m_tuning; }

                    /**
                     * Reserved bytes; supported files store zeroes here.
                     */
                    std::string reserved_zeroes() const { return m_reserved_zeroes; }

                    /**
                     * Velocity-to-volume percentage.
                     */
                    uint8_t velocity_to_volume_percent() const { return m_velocity_to_volume_percent; }

                    /**
                     * Packed 12-bit sample code pairs. This is the same little-bit-endian
                     * two-words-in-three-bytes layout used by MPC60 SET sample data.
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

#pragma once

#include "sequencer/EventState.hpp"

#include <cstdint>

namespace mpc::file::all
{
    class AllSysExEvent
    {
        static constexpr int CHUNK_HEADER_ID_OFFSET = 4;
        static constexpr int BYTE_COUNT_OFFSET = 5;
        static constexpr int DATA_OFFSET = 8;
        static constexpr int MIX_TERMINATOR_ID_OFFSET = 28;
        static constexpr int DATA_HEADER_ID_OFFSET = 8;
        static constexpr uint8_t HEADER_ID = 240;
        static constexpr uint8_t DATA_TERMINATOR_ID = 247;
        static constexpr uint8_t CHUNK_TERMINATOR_ID = 248;
        static constexpr int MIXER_SIGNATURE_OFFSET = 0;
        static constexpr int MIXER_PARAMETER_OFFSET = 5;
        static constexpr int MIXER_PAD_OFFSET = 6;
        static constexpr int MIXER_VALUE_OFFSET = 7;

        static std::vector<char> MIXER_SIGNATURE;

    public:
        static sequencer::EventState
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const sequencer::EventState &);
    };
} // namespace mpc::file::all

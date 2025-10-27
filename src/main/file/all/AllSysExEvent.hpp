#pragma once

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    class Event;
}

namespace mpc::file::all
{
    class AllSysExEvent
    {

    private:
        const static int CHUNK_HEADER_ID_OFFSET = 4;
        const static int BYTE_COUNT_OFFSET = 5;
        const static int DATA_OFFSET = 8;
        const static int MIX_TERMINATOR_ID_OFFSET = 28;
        const static int DATA_HEADER_ID_OFFSET = 8;
        static constexpr uint8_t HEADER_ID = 240;
        static constexpr uint8_t DATA_TERMINATOR_ID = 247;
        static constexpr uint8_t CHUNK_TERMINATOR_ID = 248;
        const static int MIXER_SIGNATURE_OFFSET = 0;
        const static int MIXER_PARAMETER_OFFSET = 5;
        const static int MIXER_PAD_OFFSET = 6;
        const static int MIXER_VALUE_OFFSET = 7;

        static std::vector<char> MIXER_SIGNATURE;

    public:
        static std::shared_ptr<mpc::sequencer::Event>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
            mpcEventToBytes(std::shared_ptr<mpc::sequencer::Event>);
    };
} // namespace mpc::file::all

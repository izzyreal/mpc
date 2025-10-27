#pragma once

#include <vector>
#include <memory>

namespace mpc::sequencer
{
    class ChannelPressureEvent;
}

namespace mpc::file::all
{
    class AllChannelPressureEvent
    {

    private:
        const static int AMOUNT_OFFSET = 5;

    public:
        static std::shared_ptr<mpc::sequencer::ChannelPressureEvent>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(
            std::shared_ptr<mpc::sequencer::ChannelPressureEvent>);
    };
} // namespace mpc::file::all

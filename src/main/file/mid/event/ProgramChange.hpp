#pragma once
#include "file/mid/event/ChannelEvent.hpp"

namespace mpc::file::mid::event
{
    class ProgramChange : public ChannelEvent
    {
        public:
            int getProgramNumber();
            void setProgramNumber(int p);

            ProgramChange(int tick, int channel, int program);
            ProgramChange(int tick, int delta, int channel, int program);

    };
} // namespace mpc:file::mid::event

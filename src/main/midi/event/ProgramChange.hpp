#pragma once
#include <midi/event/ChannelEvent.hpp>

namespace mpc
{
    namespace midi
    {
        namespace event
        {

            class ProgramChange
                : public ChannelEvent
            {

            public:
                virtual int getProgramNumber();
                virtual void setProgramNumber(int p);

                ProgramChange(int tick, int channel, int program);
                ProgramChange(int tick, int delta, int channel, int program);

            private:
                // friend class ProgramChange_MidiProgram;
            };

        } // namespace event
    } // namespace midi
} // namespace mpc

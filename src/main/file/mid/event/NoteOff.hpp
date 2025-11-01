#pragma once
#include "file/mid/event/ChannelEvent.hpp"

namespace mpc::file::mid::event
{
    class NoteOff : public ChannelEvent
    {
        public:
            int getNoteValue();
            int getVelocity();
            void setNoteValue(int p);
            void setVelocity(int v);

            NoteOff(int tick, int channel, int note, int velocity);
            NoteOff(int tick, int delta, int channel, int note,
                    int velocity);
    };
} // namespace mpc::file::mid::event

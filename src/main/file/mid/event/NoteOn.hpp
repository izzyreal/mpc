#pragma once
#include "file/mid/event/ChannelEvent.hpp"

namespace mpc::file::mid::event
{
    class NoteOn : public ChannelEvent
    {

    public:
        typedef ChannelEvent super;

    public:
        int getNoteValue();
        int getVelocity();
        void setNoteValue(int p);
        void setVelocity(int v);

        NoteOn(int tick, int channel, int note, int velocity);
        NoteOn(int tick, int delta, int channel, int note, int velocity);
    };
} // namespace mpc::file::mid::event

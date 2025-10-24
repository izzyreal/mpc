#pragma once
#include <midi/event/ChannelEvent.hpp>

namespace mpc
{
    namespace midi
    {
        namespace event
        {

            class NoteOn
                : public ChannelEvent
            {

            public:
                typedef ChannelEvent super;

            public:
                virtual int getNoteValue();
                virtual int getVelocity();
                virtual void setNoteValue(int p);
                virtual void setVelocity(int v);

                NoteOn(int tick, int channel, int note, int velocity);
                NoteOn(int tick, int delta, int channel, int note, int velocity);
            };

        } // namespace event
    } // namespace midi
} // namespace mpc

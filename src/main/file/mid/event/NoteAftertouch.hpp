#pragma once

#include "file/mid/event/ChannelEvent.hpp"

namespace mpc::file::mid::event
{
    class NoteAftertouch : public ChannelEvent
    {

        public:
            int getNoteValue();
            int getAmount();
            void setNoteValue(int p);
            void setAmount(int a);

            NoteAftertouch(int tick, int channel, int note, int amount);
            NoteAftertouch(int tick, int delta, int channel, int note,
                    int amount);
    };
} // namespace mpc::file::mid::event

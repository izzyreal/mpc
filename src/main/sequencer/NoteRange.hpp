#pragma once

#include "IntTypes.hpp"

namespace mpc::sequencer
{
    class NoteRange
    {
        NoteNumber start{NoNoteNumber};
        NoteNumber end{NoNoteNumber};

    public:
        NoteRange() {}
        NoteRange(const int startToUse, const int endToUse)
            : start(startToUse), end(endToUse)
        {
        }

        bool contains(const NoteNumber n) const
        {
            return n >= start && n <= end;
        }

        static NoteRange All()
        {
            return NoteRange(MinNoteNumber, MaxNoteNumber);
        }
    };
} // namespace mpc::sequencer
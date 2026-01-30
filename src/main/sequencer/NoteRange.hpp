#pragma once

#include "IntTypes.hpp"

#include <cstdint>

namespace mpc::sequencer
{
    class NoteRange
    {
        NoteNumber start{NoNoteNumber};
        NoteNumber end{NoNoteNumber};

    public:
        NoteRange() {}
        NoteRange(const int startToUse, const int endToUse)
            : start(NoteNumber(static_cast<int8_t>(startToUse))),
              end(NoteNumber(static_cast<int8_t>(endToUse)))
        {
        }

        void setStartNoteNumber(const NoteNumber startNoteNumber)
        {
            start = startNoteNumber;
        }

        void setDrumNoteNumber(const DrumNoteNumber drumNoteNumber)
        {
            if (drumNoteNumber == AllDrumNotes)
            {
                start = MinDrumNoteNumber;
                end = MaxDrumNoteNumber;
            }
            else
            {
                start = drumNoteNumber;
                end = drumNoteNumber;
            }
        }

        void setEndNoteNumber(const NoteNumber endNoteNumber)
        {
            end = endNoteNumber;
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
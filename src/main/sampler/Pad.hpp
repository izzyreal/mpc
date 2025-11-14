#pragma once

#include "IntTypes.hpp"

#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::sampler
{
    class Pad
    {
    public:
        Pad(Mpc &, ProgramPadIndex);

        void setNote(DrumNoteNumber);
        DrumNoteNumber getNote() const;
        ProgramPadIndex getIndex() const;

        static std::vector<DrumNoteNumber> &getPadNotes(const Mpc &mpcToUse);

    private:
        static std::vector<DrumNoteNumber> originalPadNotes;
        Mpc &mpc;
        DrumNoteNumber note = NoDrumNoteAssigned;
        ProgramPadIndex index{0};
    };
} // namespace mpc::sampler

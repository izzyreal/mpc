#pragma once

#include "IntTypes.hpp"

#include "performance/PerformanceMessage.hpp"

#include "utils/SmallFn.hpp"

#include <vector>
#include <functional>

namespace mpc
{
    class Mpc;
}

namespace mpc::sampler
{
    using GetPadFn = utils::SmallFn<16, performance::Pad()>;

    class Pad
    {
    public:
        Pad(Mpc &, ProgramIndex, ProgramPadIndex, GetPadFn &getSnapshot,
            const std::function<void(performance::PerformanceMessage &&)>
                &dispatch);

        void setNote(DrumNoteNumber) const;
        DrumNoteNumber getNote() const;
        ProgramPadIndex getIndex() const;

        static std::vector<DrumNoteNumber> &getPadNotes(const Mpc &mpcToUse);
        static const std::vector<DrumNoteNumber> &getOriginalPadNotes();

    private:
        static std::vector<DrumNoteNumber> originalPadNotes;
        const ProgramIndex programIndex;
        const GetPadFn getSnapshot;
        const std::function<void(performance::PerformanceMessage &&)> dispatch;
        Mpc &mpc;
        ProgramPadIndex index{0};
    };
} // namespace mpc::sampler

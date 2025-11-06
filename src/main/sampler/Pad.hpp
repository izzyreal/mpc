#pragma once

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
        Pad(Mpc &mpc, int indexToUse);

        void setNote(int i);
        int getNote() const;
        int getIndex() const;

        static std::vector<int> &getPadNotes(const Mpc &mpc);

    private:
        static std::vector<int> originalPadNotes;
        Mpc &mpc;
        int note = 0;
        int index = 0;
    };
} // namespace mpc::sampler

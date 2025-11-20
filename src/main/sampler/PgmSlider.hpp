#pragma once
#include "IntTypes.hpp"
#include "Observer.hpp"

namespace mpc::sampler
{
    class PgmSlider final : public Observable
    {
        DrumNoteNumber assignNote{NoDrumNoteAssigned};
        int tuneLowRange = -120;
        int tuneHighRange = 120;
        int decayLowRange = 12;
        int decayHighRange = 45;
        int attackLowRange = 0;
        int attackHighRange = 20;
        int filterLowRange = -50;
        int filterHighRange = 50;
        int controlChange = 0;
        NoteVariationType parameter = NoteVariationTypeTune;

    public:
        void setAssignNote(DrumNoteNumber);
        DrumNoteNumber getNote() const;
        void setTuneLowRange(int i);
        int getTuneLowRange() const;
        void setTuneHighRange(int i);
        int getTuneHighRange() const;
        void setDecayLowRange(int i);
        int getDecayLowRange() const;
        void setDecayHighRange(int i);
        int getDecayHighRange() const;
        void setAttackLowRange(int i);
        int getAttackLowRange() const;
        void setAttackHighRange(int i);
        int getAttackHighRange() const;
        void setFilterLowRange(int i);
        int getFilterLowRange() const;
        void setFilterHighRange(int i);
        int getFilterHighRange() const;
        void setControlChange(int i);
        int getControlChange() const;
        NoteVariationType getParameter() const;
        void setParameter(int i);
    };
} // namespace mpc::sampler

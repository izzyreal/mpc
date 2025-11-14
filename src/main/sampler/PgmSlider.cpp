#include "sampler/PgmSlider.hpp"

#include "IntTypes.hpp"

#include <algorithm>
#include <cassert>

using namespace mpc::sampler;

void PgmSlider::setAssignNote(const DrumNoteNumber note)
{
    assert(note >= NoDrumNoteAssigned && note <= MaxDrumNoteNumber);
    assignNote = note;
    notifyObservers("assignnote");
}

mpc::DrumNoteNumber PgmSlider::getNote() const
{
    return assignNote;
}

void PgmSlider::setTuneLowRange(int i)
{
    i = std::clamp(i, -120, 120);
    tuneLowRange = i;
    notifyObservers("lowrange");

    if (tuneLowRange > tuneHighRange)
    {
        setTuneHighRange(tuneLowRange);
    }
}

int PgmSlider::getTuneLowRange() const
{
    return tuneLowRange;
}

void PgmSlider::setTuneHighRange(int i)
{
    i = std::clamp(i, -120, 120);
    tuneHighRange = i;
    notifyObservers("highrange");

    if (tuneHighRange < tuneLowRange)
    {
        setTuneLowRange(tuneHighRange);
    }
}

int PgmSlider::getTuneHighRange() const
{
    return tuneHighRange;
}

void PgmSlider::setDecayLowRange(int i)
{
    i = std::clamp(i, 0, 100);
    decayLowRange = i;
    notifyObservers("lowrange");

    if (decayLowRange > decayHighRange)
    {
        setDecayHighRange(decayLowRange);
    }
}

int PgmSlider::getDecayLowRange() const
{
    return decayLowRange;
}

void PgmSlider::setDecayHighRange(int i)
{
    i = std::clamp(i, 0, 100);
    decayHighRange = i;
    notifyObservers("highrange");

    if (decayHighRange < decayLowRange)
    {
        setDecayLowRange(decayHighRange);
    }
}

int PgmSlider::getDecayHighRange() const
{
    return decayHighRange;
}

void PgmSlider::setAttackLowRange(int i)
{
    i = std::clamp(i, 0, 100);
    attackLowRange = i;
    notifyObservers("lowrange");

    if (attackLowRange > attackHighRange)
    {
        setAttackHighRange(attackLowRange);
    }
}

int PgmSlider::getAttackLowRange() const
{
    return attackLowRange;
}

void PgmSlider::setAttackHighRange(int i)
{
    i = std::clamp(i, 0, 100);
    attackHighRange = i;
    notifyObservers("highrange");

    if (attackHighRange < attackLowRange)
    {
        setAttackLowRange(attackHighRange);
    }
}

int PgmSlider::getAttackHighRange() const
{
    return attackHighRange;
}

void PgmSlider::setFilterLowRange(int i)
{
    i = std::clamp(i, -50, 50);
    filterLowRange = i;
    notifyObservers("lowrange");

    if (filterLowRange > filterHighRange)
    {
        setFilterHighRange(filterLowRange);
    }
}

int PgmSlider::getFilterLowRange() const
{
    return filterLowRange;
}

void PgmSlider::setFilterHighRange(int i)
{
    i = std::clamp(i, -50, 50);
    filterHighRange = i;
    notifyObservers("highrange");

    if (filterHighRange < filterLowRange)
    {
        setFilterLowRange(filterHighRange);
    }
}

int PgmSlider::getFilterHighRange() const
{
    return filterHighRange;
}

void PgmSlider::setControlChange(int i)
{
    i = std::clamp(i, 0, 128);
    controlChange = i;
    notifyObservers("controlchange");
}

int PgmSlider::getControlChange() const
{
    return controlChange;
}

int PgmSlider::getParameter() const
{
    return parameter;
}

void PgmSlider::setParameter(int i)
{
    i = std::clamp(i, 0, 3);
    parameter = i;
    notifyObservers("parameter");
}

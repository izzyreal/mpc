#include "sampler/PgmSlider.hpp"

#include <algorithm> // for std::clamp

using namespace mpc::sampler;

void PgmSlider::setAssignNote(int i)
{
    i = std::clamp(i, 34, 98);
    assignNote = i;
    notifyObservers("assignnote");
}

int PgmSlider::getNote()
{
    return assignNote;
}

void PgmSlider::setTuneLowRange(int i)
{
    i = std::clamp(i, -120, 120);
    tuneLowRange = i;
    notifyObservers("lowrange");

    if (tuneLowRange > tuneHighRange)
        setTuneHighRange(tuneLowRange);
}

int PgmSlider::getTuneLowRange()
{
    return tuneLowRange;
}

void PgmSlider::setTuneHighRange(int i)
{
    i = std::clamp(i, -120, 120);
    tuneHighRange = i;
    notifyObservers("highrange");

    if (tuneHighRange < tuneLowRange)
        setTuneLowRange(tuneHighRange);
}

int PgmSlider::getTuneHighRange()
{
    return tuneHighRange;
}

void PgmSlider::setDecayLowRange(int i)
{
    i = std::clamp(i, 0, 100);
    decayLowRange = i;
    notifyObservers("lowrange");

    if (decayLowRange > decayHighRange)
        setDecayHighRange(decayLowRange);
}

int PgmSlider::getDecayLowRange()
{
    return decayLowRange;
}

void PgmSlider::setDecayHighRange(int i)
{
    i = std::clamp(i, 0, 100);
    decayHighRange = i;
    notifyObservers("highrange");

    if (decayHighRange < decayLowRange)
        setDecayLowRange(decayHighRange);
}

int PgmSlider::getDecayHighRange()
{
    return decayHighRange;
}

void PgmSlider::setAttackLowRange(int i)
{
    i = std::clamp(i, 0, 100);
    attackLowRange = i;
    notifyObservers("lowrange");

    if (attackLowRange > attackHighRange)
        setAttackHighRange(attackLowRange);
}

int PgmSlider::getAttackLowRange()
{
    return attackLowRange;
}

void PgmSlider::setAttackHighRange(int i)
{
    i = std::clamp(i, 0, 100);
    attackHighRange = i;
    notifyObservers("highrange");

    if (attackHighRange < attackLowRange)
        setAttackLowRange(attackHighRange);
}

int PgmSlider::getAttackHighRange()
{
    return attackHighRange;
}

void PgmSlider::setFilterLowRange(int i)
{
    i = std::clamp(i, -50, 50);
    filterLowRange = i;
    notifyObservers("lowrange");

    if (filterLowRange > filterHighRange)
        setFilterHighRange(filterLowRange);
}

int PgmSlider::getFilterLowRange()
{
    return filterLowRange;
}

void PgmSlider::setFilterHighRange(int i)
{
    i = std::clamp(i, -50, 50);
    filterHighRange = i;
    notifyObservers("highrange");

    if (filterHighRange < filterLowRange)
        setFilterLowRange(filterHighRange);
}

int PgmSlider::getFilterHighRange()
{
    return filterHighRange;
}

void PgmSlider::setControlChange(int i)
{
    i = std::clamp(i, 0, 128);
    controlChange = i;
    notifyObservers("controlchange");
}

int PgmSlider::getControlChange()
{
    return controlChange;
}

int PgmSlider::getParameter()
{
    return parameter;
}

void PgmSlider::setParameter(int i)
{
    i = std::clamp(i, 0, 3);
    parameter = i;
    notifyObservers("parameter");
}


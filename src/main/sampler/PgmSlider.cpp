#include "sampler/PgmSlider.hpp"

using namespace mpc::sampler;

void PgmSlider::setAssignNote(int i)
{
    if (i < 34 || i > 98)
    {
        return;
    }

    assignNote = i;

    notifyObservers(std::string("assignnote"));
}

int PgmSlider::getNote()
{
    return assignNote;
}

void PgmSlider::setTuneLowRange(int i)
{
    if (i < -120 || i > 120)
    {
        return;
    }

    tuneLowRange = i;

    notifyObservers(std::string("lowrange"));

    if (tuneLowRange > tuneHighRange)
    {
        setTuneHighRange(tuneLowRange);
    }
}

int PgmSlider::getTuneLowRange()
{
    return tuneLowRange;
}

void PgmSlider::setTuneHighRange(int i)
{
    if (i < -120 || i > 120)
    {
        return;
    }

    tuneHighRange = i;

    notifyObservers(std::string("highrange"));
    if (tuneHighRange < tuneLowRange)
    {
        setTuneLowRange(tuneHighRange);
    }
}

int PgmSlider::getTuneHighRange()
{
    return tuneHighRange;
}

void PgmSlider::setDecayLowRange(int i)
{
    if (i < 0 || i > 100)
    {
        return;
    }

    decayLowRange = i;

    notifyObservers(std::string("lowrange"));

    if (decayLowRange > decayHighRange)
    {
        setDecayHighRange(decayLowRange);
    }
}

int PgmSlider::getDecayLowRange()
{
    return decayLowRange;
}

void PgmSlider::setDecayHighRange(int i)
{
    if (i < 0 || i > 100)
    {
        return;
    }

    decayHighRange = i;

    notifyObservers(std::string("highrange"));

    if (decayHighRange < decayLowRange)
    {
        setDecayLowRange(decayHighRange);
    }
}

int PgmSlider::getDecayHighRange()
{
    return decayHighRange;
}

void PgmSlider::setAttackLowRange(int i)
{
    if (i < 0 || i > 100)
    {
        return;
    }

    attackLowRange = i;

    notifyObservers(std::string("lowrange"));

    if (attackLowRange > attackHighRange)
    {
        setAttackHighRange(attackLowRange);
    }
}

int PgmSlider::getAttackLowRange()
{
    return attackLowRange;
}

void PgmSlider::setAttackHighRange(int i)
{
    if (i < 0 || i > 100)
    {
        return;
    }

    attackHighRange = i;

    notifyObservers(std::string("highrange"));

    if (attackHighRange < attackLowRange)
    {
        setAttackLowRange(attackHighRange);
    }
}

int PgmSlider::getAttackHighRange()
{
    return attackHighRange;
}

void PgmSlider::setFilterLowRange(int i)
{
    if (i < -50 || i > 50)
    {
        return;
    }

    filterLowRange = i;

    notifyObservers(std::string("lowrange"));

    if (filterLowRange > filterHighRange)
    {
        setFilterHighRange(filterLowRange);
    }
}

int PgmSlider::getFilterLowRange()
{
    return filterLowRange;
}

void PgmSlider::setFilterHighRange(int i)
{
    if (i < -50 || i > 50)
    {
        return;
    }

    filterHighRange = i;

    notifyObservers(std::string("highrange"));

    if (filterHighRange < filterLowRange)
    {
        setFilterLowRange(filterHighRange);
    }
}

int PgmSlider::getFilterHighRange()
{
    return filterHighRange;
}

void PgmSlider::setControlChange(int i)
{
    if (i < 0 || i > 128)
    {
        return;
    }

    controlChange = i;

    notifyObservers(std::string("controlchange"));
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
    if (i < 0 || i > 3)
    {
        return;
    }

    parameter = i;

    notifyObservers(std::string("parameter"));
}

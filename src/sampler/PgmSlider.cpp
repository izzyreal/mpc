#include <sampler/PgmSlider.hpp>

using namespace mpc::sampler;
using namespace std;

PgmSlider::PgmSlider()
{
}

void PgmSlider::setAssignNote(int i)
{
    if(i < 34 || i > 98) return;

    assignNote = i;
    setChanged();
    notifyObservers(string("assignnote"));
}

int PgmSlider::getNote()
{
    return assignNote;
}

void PgmSlider::setTuneLowRange(int i)
{
    if(i < -120 || i > 120)
        return;

    tuneLowRange = i;
    setChanged();
    notifyObservers(string("lowrange"));
    if(tuneLowRange > tuneHighRange)
        setTuneHighRange(tuneLowRange);

}

int PgmSlider::getTuneLowRange()
{
    return tuneLowRange;
}

void PgmSlider::setTuneHighRange(int i)
{
    if(i < -120 || i > 120)
        return;

    tuneHighRange = i;
    setChanged();
    notifyObservers(string("highrange"));
    if(tuneHighRange < tuneLowRange)
        setTuneLowRange(tuneHighRange);

}

int PgmSlider::getTuneHighRange()
{
    return tuneHighRange;
}

void PgmSlider::setDecayLowRange(int i)
{
    if(i < 0 || i > 100)
        return;

    decayLowRange = i;
    setChanged();
    notifyObservers(string("lowrange"));
    if(decayLowRange > decayHighRange)
        setDecayHighRange(decayLowRange);

}

int PgmSlider::getDecayLowRange()
{
    return decayLowRange;
}

void PgmSlider::setDecayHighRange(int i)
{
    if(i < 0 || i > 100)
        return;

    decayHighRange = i;
    setChanged();
    notifyObservers(string("highrange"));
    if(decayHighRange < decayLowRange)
        setDecayLowRange(decayHighRange);

}

int PgmSlider::getDecayHighRange()
{
    return decayHighRange;
}

void PgmSlider::setAttackLowRange(int i)
{
    if(i < 0 || i > 100)
        return;

    attackLowRange = i;
    setChanged();
    notifyObservers(string("lowrange"));
    if(attackLowRange > attackHighRange)
        setAttackHighRange(attackLowRange);

}

int PgmSlider::getAttackLowRange()
{
    return attackLowRange;
}

void PgmSlider::setAttackHighRange(int i)
{
    if(i < 0 || i > 100)
        return;

    attackHighRange = i;
    setChanged();
    notifyObservers(string("highrange"));
    if(attackHighRange < attackLowRange)
        setAttackLowRange(attackHighRange);

}

int PgmSlider::getAttackHighRange()
{
    return attackHighRange;
}

void PgmSlider::setFilterLowRange(int i)
{
    if(i < -50 || i > 50)
        return;

    filterLowRange = i;
    setChanged();
    notifyObservers(string("lowrange"));
    if(filterLowRange > filterHighRange)
        setFilterHighRange(filterLowRange);

}

int PgmSlider::getFilterLowRange()
{
    return filterLowRange;
}

void PgmSlider::setFilterHighRange(int i)
{
    if(i < -50 || i > 50)
        return;

    filterHighRange = i;
    setChanged();
    notifyObservers(string("highrange"));
    if(filterHighRange < filterLowRange)
        setFilterLowRange(filterHighRange);

}

int PgmSlider::getFilterHighRange()
{
    return filterHighRange;
}

void PgmSlider::setControlChange(int i)
{
    if(i < 0 || i > 128)
        return;

    controlChange = i;
    setChanged();
    notifyObservers(string("controlchange"));
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
    if(i < 0 || i > 3)
        return;

    parameter = i;
    setChanged();
    notifyObservers(string("parameter"));
}

PgmSlider::~PgmSlider() {
	// nothing to destroy
}

#include "FilterControls.hpp"

#include "engine/control/Control.hpp"
#include "engine/control/LawControl.hpp"
#include "engine/control/LinearLaw.hpp"

using namespace mpc::engine::filter;
using namespace mpc::engine::control;
using namespace std;

FilterControls::FilterControls(int id, const string &name, int idOffset)
    : CompoundControl(id, name)
{
    this->idOffset = idOffset;
    createControls();
    deriveSampleRateIndependentVariables();
}

shared_ptr<ControlLaw> FilterControls::SEMITONE_LAW()
{
    static shared_ptr<ControlLaw> res =
        make_shared<LinearLaw>(-48.f, 96.f, "semitones");
    return res;
}

void FilterControls::derive(Control *c)
{
    switch (c->getId() - idOffset)
    {
        case FREQUENCY:
            cutoff = deriveCutoff();
            break;
        case RESONANCE:
            resonance = deriveResonance();
            break;
        default:;
    }
}

void FilterControls::createControls()
{
    cutoffControl = createCutoffControl();
    add(shared_ptr<Control>(cutoffControl));
    resonanceControl = createResonanceControl();
    add(shared_ptr<Control>(resonanceControl));
}

void FilterControls::deriveSampleRateIndependentVariables()
{
    resonance = deriveResonance();
    cutoff = deriveCutoff();
}

float FilterControls::deriveResonance()
{
    return resonanceControl->getValue();
}

float FilterControls::deriveCutoff()
{
    return cutoffControl->getValue();
}

LawControl *FilterControls::createCutoffControl()
{
    return new LawControl(FREQUENCY + idOffset, "Cutoff", SEMITONE_LAW(), 0.0f);
}

LawControl *FilterControls::createResonanceControl()
{
    return new LawControl(RESONANCE + idOffset, "Resonance", LinearLaw::UNITY(),
                          0.25f);
}

float FilterControls::getCutoff() const
{
    return cutoff;
}

float FilterControls::getResonance() const
{
    return resonance;
}

#include "StateVariableFilterElement.hpp"

using namespace ctoot::synth::modules::filter;

float StateVariableFilterElement::filter(float in, float freq, float damp)
{
    auto i1 = (prev + in) * 0.5f;
    prev = in;
    notch = i1 - damp * band;
    low = low + freq * band;
    high = notch - low;
    band = freq * high + band;
    notch = in - damp * band;
    low = low + freq * band;
    high = notch - low;
    band = freq * high + band;
    return bp ? band : (1.0f - mix) * low + mix * high;
}

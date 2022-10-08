#include <file/pgmwriter/PWSlider.hpp>

#include <sampler/Program.hpp>

using namespace mpc::file::pgmwriter;
using namespace std;

Slider::Slider(mpc::sampler::Program* program) 
{
	sliderArray = vector<char>(15);
    if(program->getSlider()->getNote() == 34) {
        setMidiNoteAssign(0);
    } else {
        setMidiNoteAssign(program->getSlider()->getNote());
    }
    setTuneLow(program->getSlider()->getTuneLowRange());
    setTuneHigh(program->getSlider()->getTuneHighRange());
    setDecayLow(program->getSlider()->getDecayLowRange());
    setDecayHigh(program->getSlider()->getDecayHighRange());
    setAttackLow(program->getSlider()->getAttackLowRange());
    setAttackHigh(program->getSlider()->getAttackHighRange());
    setFilterLow(program->getSlider()->getFilterLowRange());
    setFilterHigh(program->getSlider()->getFilterHighRange());
    setControlChange(program->getSlider()->getControlChange());
    sliderArray[10] = 35;
    sliderArray[11] = 64;
    sliderArray[12] = 0;
    sliderArray[13] = 25;
    sliderArray[14] = 0;
}

vector<char> Slider::getSliderArray()
{
    return sliderArray;
}

void Slider::setMidiNoteAssign(int midiNoteAssign)
{
    sliderArray[0] = midiNoteAssign;
}

void Slider::setTuneLow(int tuneLow)
{
    sliderArray[1] = tuneLow;
}

void Slider::setTuneHigh(int tuneHigh)
{
    sliderArray[2] = tuneHigh;
}

void Slider::setDecayLow(int decayLow)
{
    sliderArray[3] = decayLow;
}

void Slider::setDecayHigh(int decayHigh)
{
    sliderArray[4] = decayHigh;
}

void Slider::setAttackLow(int attackLow)
{
    sliderArray[5] = attackLow;
}

void Slider::setAttackHigh(int attackHigh)
{
    sliderArray[6] = attackHigh;
}

void Slider::setFilterLow(int filterLow)
{
    sliderArray[7] = filterLow;
}

void Slider::setFilterHigh(int filterHigh)
{
    sliderArray[8] = filterHigh;
}

void Slider::setControlChange(int controlChange)
{
    sliderArray[9] = controlChange;
}

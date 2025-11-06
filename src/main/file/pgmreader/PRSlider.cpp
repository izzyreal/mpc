#include "file/pgmreader/PRSlider.hpp"

#include "file/pgmreader/ProgramFileReader.hpp"
#include "file/pgmreader/SoundNames.hpp"

#include "Util.hpp"

using namespace mpc::file::pgmreader;

Slider::Slider(ProgramFileReader *programFile)
{
    this->programFile = programFile;
}

int Slider::getSampleNamesSize()
{
    sampleNamesSize = programFile->getSampleNames()->getSampleNamesSize();
    return sampleNamesSize;
}

int Slider::getSliderStart()
{
    int sliderStart = 4 + getSampleNamesSize() + 2 + 17;
    return sliderStart;
}

int Slider::getSliderEnd()
{
    auto sliderEnd = 4 + getSampleNamesSize() + 2 + 17 + 10 + 5;
    return sliderEnd;
}

std::vector<char> Slider::getSliderArray()
{
    auto pgmFileArray = programFile->readProgramFileArray();
    sliderArray =
        Util::vecCopyOfRange(pgmFileArray, getSliderStart(), getSliderEnd());
    return sliderArray;
}

int Slider::getMidiNoteAssign()
{
    auto midiNoteAssign = getSliderArray()[0];
    return midiNoteAssign;
}

int Slider::getTuneLow()
{
    auto tuneLow = getSliderArray()[1];
    return tuneLow;
}

int Slider::getTuneHigh()
{
    auto tuneHigh = getSliderArray()[2];
    return tuneHigh;
}

int Slider::getDecayLow()
{
    auto decayLow = getSliderArray()[3];
    return decayLow;
}

int Slider::getDecayHigh()
{
    auto decayHigh = getSliderArray()[4];
    return decayHigh;
}

int Slider::getAttackLow()
{
    auto attackLow = getSliderArray()[5];
    return attackLow;
}

int Slider::getAttackHigh()
{
    auto attackHigh = getSliderArray()[6];
    return attackHigh;
}

int Slider::getFilterLow()
{
    auto filterLow = getSliderArray()[7];
    return filterLow;
}

int Slider::getFilterHigh()
{
    auto filterHigh = getSliderArray()[8];
    return filterHigh;
}

int Slider::getControlChange()
{
    auto controlChange = getSliderArray()[9];
    return controlChange;
}

#include "file/aps/ApsSlider.hpp"

#include "file/aps/ApsProgram.hpp"
#include "sampler/PgmSlider.hpp"

using namespace mpc::file::aps;

ApsSlider::ApsSlider(const std::vector<char> &loadBytes)
{
    note = loadBytes[0] == 0 ? 34 : loadBytes[0];
    tuneLow = loadBytes[1];
    tuneHigh = loadBytes[2];
    decayLow = loadBytes[3];
    decayHigh = loadBytes[4];
    attackLow = loadBytes[5];
    attackHigh = loadBytes[6];
    filterLow = loadBytes[7];
    filterHigh = loadBytes[8];
    programChange = loadBytes[9];
}

ApsSlider::ApsSlider(sampler::PgmSlider *slider)
{
    saveBytes = std::vector<char>(ApsProgram::SLIDER_LENGTH);
    saveBytes[0] = slider->getNote() == 34 ? 0 : slider->getNote();
    saveBytes[1] = slider->getTuneLowRange();
    saveBytes[2] = slider->getTuneHighRange();
    saveBytes[3] = slider->getDecayLowRange();
    saveBytes[4] = slider->getDecayHighRange();
    saveBytes[5] = slider->getAttackLowRange();
    saveBytes[6] = slider->getAttackHighRange();
    saveBytes[7] = slider->getFilterLowRange();
    saveBytes[8] = slider->getFilterHighRange();
    saveBytes[9] = slider->getControlChange();
}

std::vector<char> ApsSlider::PADDING = std::vector<char>{0, 35, 64, 0, 26, 0};

int ApsSlider::getNote() const
{
    return note;
}

int ApsSlider::getTuneLow() const
{
    return tuneLow;
}

int ApsSlider::getTuneHigh() const
{
    return tuneHigh;
}

int ApsSlider::getDecayLow() const
{
    return decayLow;
}

int ApsSlider::getDecayHigh() const
{
    return decayHigh;
}

int ApsSlider::getAttackLow() const
{
    return attackLow;
}

int ApsSlider::getAttackHigh() const
{
    return attackHigh;
}

int ApsSlider::getFilterLow() const
{
    return filterLow;
}

int ApsSlider::getFilterHigh() const
{
    return filterHigh;
}

int ApsSlider::getProgramChange() const
{
    return programChange;
}

std::vector<char> ApsSlider::getBytes()
{
    return saveBytes;
}

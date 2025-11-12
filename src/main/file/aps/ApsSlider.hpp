#pragma once

#include <vector>

namespace mpc::sampler
{
    class PgmSlider;
}

namespace mpc::file::aps
{
    class ApsSlider
    {

    public:
        int note;
        int tuneLow;
        int tuneHigh;
        int decayLow;
        int decayHigh;
        int attackLow;
        int attackHigh;
        int filterLow;
        int filterHigh;
        int programChange;
        std::vector<char> saveBytes;

    private:
        static std::vector<char> PADDING;

    public:
        int getNote() const;
        int getTuneLow() const;
        int getTuneHigh() const;
        int getDecayLow() const;
        int getDecayHigh() const;
        int getAttackLow() const;
        int getAttackHigh() const;
        int getFilterLow() const;
        int getFilterHigh() const;
        int getProgramChange() const;
        std::vector<char> getBytes();

        ApsSlider(const std::vector<char> &loadBytes);
        ApsSlider(sampler::PgmSlider *slider);
    };
} // namespace mpc::file::aps

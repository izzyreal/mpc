#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{
    class MixerFader
        : public Component
    {

    private:
        int value{100};
        bool color = true;

    public:
        void setValue(int value);
        void setColor(bool on);

        void Draw(std::vector<std::vector<bool>> *pixels) override;

        MixerFader(MRECT rect);
    };
} // namespace mpc::lcdgui

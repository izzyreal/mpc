#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class ConvertSoundScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        ConvertSoundScreen(mpc::Mpc &mpc, int layerIndex);

        void open() override;

    private:
        const std::vector<std::string> convertNames{"STEREO TO MONO",
                                                    "RE-SAMPLE"};
        void displayConvert();

        int convert = 0;
        void setConvert(int i);
    };
} // namespace mpc::lcdgui::screens::dialog

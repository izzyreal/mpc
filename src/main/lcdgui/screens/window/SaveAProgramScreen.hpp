#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class SaveApsFileScreen;
}

namespace mpc::disk
{
    class ApsSaver;
    class AbstractDisk;
    class SoundSaver;
} // namespace mpc::disk

namespace mpc::lcdgui::screens::window
{
    class SaveAProgramScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        SaveAProgramScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;

    private:
        const std::vector<std::string> pgmSaveNames{"PROGRAM ONLY", "WITH SOUNDS", "WITH .WAV"};

        bool replaceSameSounds = false;

        int save = 1;
        void setSave(int i);

        void displaySave();
        void displayFile();
        void displayReplaceSameSounds();

    private:
        friend class SaveApsFileScreen;
        friend class mpc::disk::ApsSaver;
        friend class mpc::disk::SoundSaver;
        friend class mpc::disk::AbstractDisk;
    };
} // namespace mpc::lcdgui::screens::window

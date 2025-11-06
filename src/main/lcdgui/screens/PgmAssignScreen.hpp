#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::sampler
{
    class Pad;
}

namespace mpc::file::aps
{
    class ApsGlobalParameters;
}

namespace mpc::lcdgui::screens
{

    class PgmAssignScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;
        void openWindow() override;

        PgmAssignScreen(mpc::Mpc &mpc, int layerIndex);
        void update(Observable *o, Message message) override;
        void open() override;
        void close() override;

        void setPadAssign(bool isMaster);

    private:
        const std::vector<std::string> soundGenerationModes{"NORMAL", "SIMULT",
                                                            "VEL SW", "DCY SW"};
        bool padAssign = false;
        void displayPgm();
        void displaySoundName();
        void displayPadAssign();
        void displayPadNote();
        void displaySoundGenerationMode();
        void displayVeloRangeUpper();
        void displayVeloRangeLower();
        void displayOptionalNoteB();
        void displayOptionalNoteA();
        void displayNote();
        void displayPad();

    private:
        friend class mpc::sampler::Pad;
        friend class mpc::file::aps::ApsGlobalParameters;
    };
} // namespace mpc::lcdgui::screens

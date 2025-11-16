#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class PgmAssignScreen final : public ScreenComponent
    {
    public:
        PgmAssignScreen(Mpc &mpc, int layerIndex);

        void function(int i) override;
        void turnWheel(int i) override;
        void openWindow() override;

        void open() override;

        void setPadAssign(bool isMaster);

        bool isPadAssignMaster() const;

    private:
        const std::vector<std::string> soundGenerationModes{"NORMAL", "SIMULT",
                                                            "VEL SW", "DCY SW"};
        bool padAssign = false;
        void displayPgm() const;
        void displaySoundName() const;
        void displayPadAssign() const;
        void displayPadNote() const;
        void displaySoundGenerationMode();
        void displayVeloRangeUpper() const;
        void displayVeloRangeLower() const;
        void displayOptionalNoteB() const;
        void displayOptionalNoteA() const;
        void displayNote() const;
        void displayPad() const;
    };
} // namespace mpc::lcdgui::screens

#pragma once
#include "lcdgui/ScreenComponent.hpp"

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

    class PgmAssignScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;
        void openWindow() override;

        PgmAssignScreen(Mpc &mpc, int layerIndex);
        void update(Observable *o, Message message) override;
        void open() override;
        void close() override;

        void setPadAssign(bool isMaster);

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

        friend class sampler::Pad;
        friend class file::aps::ApsGlobalParameters;
    };
} // namespace mpc::lcdgui::screens

#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{

    class PgmParamsScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;
        void openWindow() override;

        PgmParamsScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;
        void update(Observable *observable, Message message) override;

    private:
        void displayReson() const;
        void displayFreq() const;
        void displayAttackDecay();
        void displayNote() const;
        void displayPgm() const;
        void displayTune() const;
        void displayDecayMode();
        void displayVoiceOverlap() const;

        std::vector<std::string> decayModes = {"END", "START"};
        std::vector<std::string> voiceOverlapModes = {"POLY", "MONO",
                                                      "NOTE OFF"};
    };
} // namespace mpc::lcdgui::screens

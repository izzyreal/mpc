#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class SaveAProgramScreen final : public ScreenComponent
    {
    public:
        void turnWheel(int i) override;
        void function(int i) override;

        SaveAProgramScreen(Mpc &mpc, int layerIndex);
        void open() override;

        bool replaceSameSounds = false;
        int save = 1;
        void setSave(int i);

    private:
        const std::vector<std::string> pgmSaveNames{"PROGRAM ONLY",
                                                    "WITH SOUNDS", "WITH .WAV"};

        void displaySave() const;
        void displayFile() const;
        void displayReplaceSameSounds() const;
    };
} // namespace mpc::lcdgui::screens::window

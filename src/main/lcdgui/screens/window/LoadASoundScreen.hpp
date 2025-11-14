#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class LoadASoundScreen final : public ScreenComponent
    {

        void keepSound() const;

    public:
        LoadASoundScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;

        void numpad(int i) override {}

        void update(Observable *observable, Message message) override;

        void turnWheel(int i) override;
        void function(int i) override;

    private:
        void displayAssignToNote() const;
        DrumNoteNumber assignToNote{60};
    };
} // namespace mpc::lcdgui::screens::window

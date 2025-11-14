#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class KeepOrRetryScreen final : public ScreenComponent,
                                    public OpensNameScreen
    {

    public:
        KeepOrRetryScreen(Mpc &, int layerIndex);

        void update(Observable *, Message) override;

        void open() override;
        void close() override;
        void function(int i) override;
        void turnWheel(int increment) override;
        void right() override;

    private:
        void displayNameForNewSound() const;
        void displayAssignToNote() const;
        void openNameScreen() override;
        DrumNoteNumber assignToNote = NoDrumNoteAssigned;
    };
} // namespace mpc::lcdgui::screens::window

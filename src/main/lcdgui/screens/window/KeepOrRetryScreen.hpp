#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class KeepOrRetryScreen final : public ScreenComponent,
                                    public OpensNameScreen
    {

    public:
        KeepOrRetryScreen(Mpc &mpc, int layerIndex);

        void update(Observable *o, Message message) override;

        void open() override;
        void close() override;
        void function(int i) override;
        void turnWheel(int i) override;
        void right() override;

    private:
        void displayNameForNewSound() const;
        void displayAssignToNote() const;
        void openNameScreen() override;
        unsigned char assignToNote = 34;
    };
} // namespace mpc::lcdgui::screens::window

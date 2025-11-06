#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class MuteAssignScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        MuteAssignScreen(Mpc &mpc, int layerIndex);
        void open() override;
        void close() override;
        void update(Observable *o, Message message) override;

    private:
        void displayNote() const;
        void displayNote0() const;
        void displayNote1() const;
    };
} // namespace mpc::lcdgui::screens::window

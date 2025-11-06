#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class IgnoreTempoChangeScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;
        void turnWheel(int i) override;

        IgnoreTempoChangeScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void setIgnore(bool);
        bool getIgnore() const;

    private:
        bool ignore = true;
        void displayIgnore() const;
    };
} // namespace mpc::lcdgui::screens::window

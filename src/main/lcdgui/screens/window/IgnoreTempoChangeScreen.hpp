#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::lcdgui::screens::window
{
    class IgnoreTempoChangeScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        IgnoreTempoChangeScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

        void setIgnore(bool);
        bool getIgnore();

    private:
        bool ignore = true;
        void displayIgnore();

        friend class mpc::sequencer::Sequencer;
    };
} // namespace mpc::lcdgui::screens::window

#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <vector>

namespace mpc::lcdgui
{
    class HorizontalBar2;
}

namespace mpc::sequencer
{
    class TempoChangeEvent;
}

namespace mpc::lcdgui::screens::window
{

    class TempoChangeScreen
        : public mpc::lcdgui::ScreenComponent
    {

    private:
        std::vector<std::shared_ptr<mpc::lcdgui::HorizontalBar2>> bars;
        std::vector<std::shared_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChanges;

    private:
        void initVisibleEvents();
        void displayTempoChange0();
        void displayTempoChange1();
        void displayTempoChange2();
        void displayTempoChangeOn();
        void displayInitialTempo();

    public:
        TempoChangeScreen(mpc::Mpc &mpc, const int layerIndex);

    public:
        void open() override;

    public:
        void left() override;
        void right() override;
        void function(int j) override;
        void turnWheel(int j) override;
        void down() override;
        void up() override;

    private:
        std::weak_ptr<mpc::sequencer::TempoChangeEvent> previous;
        std::weak_ptr<mpc::sequencer::TempoChangeEvent> current;
        std::weak_ptr<mpc::sequencer::TempoChangeEvent> next;
        int offset = 0;
        void setOffset(int i);

        void init();
    };
} // namespace mpc::lcdgui::screens::window

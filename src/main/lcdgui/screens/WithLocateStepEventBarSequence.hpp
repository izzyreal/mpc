#pragma once

namespace mpc::hardware
{
    class Button;
}

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::lcdgui::screens
{
    class WithLocateStepEventBarSequence
    {
    public:
        virtual void prevStepEvent();
        virtual void nextStepEvent();
        virtual void prevBarStart();
        virtual void nextBarEnd();

        virtual ~WithLocateStepEventBarSequence() = default;

    private:
        WithLocateStepEventBarSequence() = delete;
        hardware::Button *goToButton;
        sequencer::Sequencer *mySequencer;

    protected:
        WithLocateStepEventBarSequence(hardware::Button *goToButton,
                                       sequencer::Sequencer *);
    };
} // namespace mpc::lcdgui::screens
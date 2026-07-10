#include "LoadASequencePlayScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Transport.hpp"

using namespace mpc::lcdgui::screens::window;

LoadASequencePlayScreen::LoadASequencePlayScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-a-sequence-play", layerIndex)
{
    findBackground()->setBackgroundName("step-timing-correct");

    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getTransport()
                                ->getTickPositionGuiPresentation();
                        },
                        [&](auto)
                        {
                            displayPosition();
                        }});
}

void LoadASequencePlayScreen::open()
{
    displayPosition();
}

void LoadASequencePlayScreen::displayPosition() const
{
    const auto transport = sequencer.lock()->getTransport();

    findLabel("position")->setText(
        StrUtil::padLeft(
            std::to_string(transport->getCurrentBarIndex() + 1), "0", 3) +
        "." +
        StrUtil::padLeft(
            std::to_string(transport->getCurrentBeatIndex() + 1), "0", 2) +
        "." +
        StrUtil::padLeft(
            std::to_string(transport->getCurrentClockNumber()), "0", 2));
}

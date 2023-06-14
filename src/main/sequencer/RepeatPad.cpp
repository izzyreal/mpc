#include "RepeatPad.hpp"

#include "Mpc.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "controls/GlobalReleaseControls.hpp"

using namespace mpc::sequencer;

void RepeatPad::process(mpc::Mpc& mpc,
                        unsigned int tickPosition,
                        int durationTicks,
                        unsigned short eventFrameOffset,
                        double tempo,
                        float sampleRate)
{
    auto sequencer = mpc.getSequencer();
    auto hardware = mpc.getHardware();

    for (auto& p : hardware->getPads())
    {
        if (!p->isPressed()) continue;

        const auto pressure = p->getPressure();
        mpc.getReleaseControls()->simplePad(p->getPadIndexWithBankWhenLastPressed());
        mpc.getControls()->pressPad(p->getPadIndexWithBankWhenLastPressed());
        mpc.getActiveControls()->pad(p->getPadIndexWithBankWhenLastPressed(), pressure);
    }
}
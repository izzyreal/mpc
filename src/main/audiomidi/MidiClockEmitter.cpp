#include "MidiClockEmitter.hpp"

#include "Mpc.hpp"
#include "audiomidi/MpcMidiOutput.hpp"
#include "lcdgui/screens/SyncScreen.hpp"

#include "midi/core/ShortMessage.hpp"

using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace ctoot::midi::core;

MidiClockEmitter::MidiClockEmitter(mpc::Mpc &mpcToUse) :
        mpc(mpcToUse),
        msg(std::make_shared<ShortMessage>()),
        syncScreen(mpc.screens->get<SyncScreen>("sync"))
{
    msg->setMessage(ShortMessage::TIMING_CLOCK);
}

void MidiClockEmitter::work(int nFrames)
{
    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++) {
        if (clock.proc()) {
            tickCounter++;
            if (tickCounter >= 4)
            {
                tickCounter = 0;
                sendMidiClockMsg(frameIndex);
            }
        }
    }
}

void MidiClockEmitter::sendMidiClockMsg(unsigned int frameOffset)
{
    if (syncScreen->getModeOut() > 0) {
        msg->bufferPos = frameOffset;

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2) {
            mpc.getMidiOutput()->enqueMessageOutputA(msg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2) {
            mpc.getMidiOutput()->enqueMessageOutputB(msg);
        }
    }
}

void MidiClockEmitter::setSampleRate(unsigned int sampleRate)
{
    clock.init(sampleRate);
}

void MidiClockEmitter::setTempo(float bpm)
{
    clock.set_bpm(bpm);
}

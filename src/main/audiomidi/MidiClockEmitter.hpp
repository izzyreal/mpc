#pragma once

#include "audio/server/AudioClient.hpp"
#include "sequencer/Clock.hpp"

#include <memory>

namespace ctoot::midi::core { class ShortMessage; }

namespace mpc { class Mpc; }
namespace mpc::lcdgui::screens { class SyncScreen; }

namespace mpc::audiomidi {

    class MidiClockEmitter : public ctoot::audio::server::AudioClient
    {
    public:
        MidiClockEmitter(mpc::Mpc &);

        void work(int nFrames) override;

        void setEnabled(bool) override
        {};

        void setSampleRate(unsigned int sampleRate);
        void setTempo(float bpm);

    private:
        mpc::Mpc &mpc;
        unsigned char tickCounter = 0;
        mpc::sequencer::Clock clock;
        std::shared_ptr<ctoot::midi::core::ShortMessage> msg;
        std::shared_ptr<mpc::lcdgui::screens::SyncScreen> syncScreen;

        void sendMidiClockMsg(unsigned int frameOffset);

    };

}
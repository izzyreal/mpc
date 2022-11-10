#pragma once

#include <observer/Observable.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <midi/core/MidiInput.hpp>
#include <sequencer/EventAdapter.hpp>
#include <sequencer/MidiAdapter.hpp>
#include "VmpcMidiControlMode.hpp"

#include <memory>

namespace ctoot::midi::core { class ShortMessage; }

namespace mpc::audiomidi {
    class MpcMidiPorts;
}

namespace mpc::audiomidi {
    class MpcMidiInput
            : public moduru::observer::Observable, public virtual ctoot::midi::core::MidiInput
    {

    public:
        int index = 0;

    private:
        mpc::Mpc &mpc;
        std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
        std::weak_ptr<mpc::sampler::Sampler> sampler;

        std::unique_ptr<mpc::sequencer::MidiAdapter> midiAdapter;
        std::unique_ptr<mpc::sequencer::EventAdapter> eventAdapter;
        std::unique_ptr<VmpcMidiControlMode> midiFullControl;
        std::string notify = "";

    public:
        std::string getName() override;

        void transport(ctoot::midi::core::MidiMessage *msg, int timestamp) override;

    private:
        void midiOut(std::weak_ptr<sequencer::Event> event, sequencer::Track *track);

        void transportOmni(ctoot::midi::core::MidiMessage *msg, std::string outputLetter);

        void handleControl(ctoot::midi::core::ShortMessage *shortMsg);

        void handlePolyAndNote(ctoot::midi::core::MidiMessage *msg);

    public:
        MpcMidiInput(mpc::Mpc &mpc, int index);

    };
}

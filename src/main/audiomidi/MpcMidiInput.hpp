#pragma once

#include <observer/Observable.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/EventAdapter.hpp>
#include "VmpcMidiControlMode.hpp"

#include <memory>

namespace mpc::engine::midi { class ShortMessage; }

namespace mpc::audiomidi {
    class MpcMidiOutput;
}

namespace mpc::audiomidi {
    class MpcMidiInput
            : public moduru::observer::Observable
    {

    public:
        int index = 0;

    private:
        mpc::Mpc &mpc;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::sampler::Sampler> sampler;
        std::unique_ptr<mpc::sequencer::EventAdapter> eventAdapter;
        std::unique_ptr<VmpcMidiControlMode> midiFullControl;
        std::string notify;

    public:
        std::string getName();

        void transport(mpc::engine::midi::MidiMessage *msg, int timestamp);

    private:
        void midiOut(std::weak_ptr<sequencer::Event> event, sequencer::Track *track);

        void transportOmni(mpc::engine::midi::MidiMessage *msg, std::string outputLetter);

        void handleControl(mpc::engine::midi::ShortMessage *shortMsg);

        void handlePolyAndNote(mpc::engine::midi::MidiMessage *msg);

    public:
        MpcMidiInput(mpc::Mpc &mpc, int index);

    };
}

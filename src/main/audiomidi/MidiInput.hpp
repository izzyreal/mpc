#pragma once

#include <Observer.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <utility>
#include "VmpcMidiControlMode.hpp"
#include "sequencer/NoteEventStore.hpp"

#include <memory>

namespace mpc::engine::midi { class ShortMessage; }

namespace mpc::audiomidi {
    class MidiOutput;
}

namespace mpc::audiomidi {
    class MidiInput :
        public Observable,
        public mpc::sequencer::NoteEventStore<std::pair<int/*track*/,int/*note*/>>
    {
    private:
        int index = 0;
        mpc::Mpc &mpc;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::sampler::Sampler> sampler;
        std::unique_ptr<VmpcMidiControlMode> midiFullControl;
        std::string notify;

    public:
        void transport(mpc::engine::midi::MidiMessage *msg, int timestamp);

    private:
        void midiOut(sequencer::Track* track);

        void transportOmni(mpc::engine::midi::MidiMessage *msg, const std::string& outputLetter);
        
        std::shared_ptr<mpc::sequencer::NoteOnEvent> handleNoteOn(mpc::engine::midi::ShortMessage* msg, const int& timeStamp);
        std::shared_ptr<mpc::sequencer::NoteOffEvent> handleNoteOff(mpc::engine::midi::ShortMessage* msg, const int& timeStamp);

        std::shared_ptr<mpc::sequencer::MidiClockEvent> handleMidiClock(mpc::engine::midi::ShortMessage* msg);

        void handleControlChange(mpc::engine::midi::ShortMessage* msg);

        void handleChannelPressure(mpc::engine::midi::ShortMessage* msg);

    public:
        MidiInput(mpc::Mpc &mpc, int index);

    };
}

#pragma once

#include "MidiClockInput.hpp"

#include <observer/Observable.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <utility>
#include "VmpcMidiControlMode.hpp"
#include "sequencer/NoteEventStore.hpp"

#include <memory>

namespace mpc::engine::midi { class ShortMessage; }

namespace mpc::audiomidi {
    class MpcMidiOutput;
}

namespace mpc::audiomidi {
    class MpcMidiInput : 
        public moduru::observer::Observable,
        public mpc::sequencer::NoteEventStore<std::pair<int/*track*/,int/*note*/>, mpc::sequencer::NoteOnEventPlayOnly>
    {
    private:
        int index = 0;
        mpc::Mpc &mpc;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::sampler::Sampler> sampler;
        std::unique_ptr<VmpcMidiControlMode> midiFullControl;
        std::string notify;
        MidiClockInput midiClockInput;

    public:
        void transport(mpc::engine::midi::MidiMessage *msg, int timestamp);

    private:
        void midiOut(std::weak_ptr<sequencer::Event> event, sequencer::Track *track);

        void transportOmni(mpc::engine::midi::MidiMessage *msg, std::string outputLetter);
        
        std::shared_ptr<mpc::sequencer::NoteOnEvent> handleNoteOn(mpc::engine::midi::ShortMessage* msg, const int& timeStamp);
        std::shared_ptr<mpc::sequencer::NoteOffEvent> handleNoteOff(mpc::engine::midi::ShortMessage* msg, const int& timeStamp);

        std::shared_ptr<mpc::sequencer::MidiClockEvent> handleMidiClock(mpc::engine::midi::ShortMessage* msg, const int bufOffset);

        void handleControlChange(mpc::engine::midi::ShortMessage* msg);

        void handleChannelPressure(mpc::engine::midi::ShortMessage* msg);

    public:
        MpcMidiInput(mpc::Mpc &mpc, int index);

    };
}

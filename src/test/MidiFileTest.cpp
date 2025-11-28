#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"

#include "file/mid/MidiReader.hpp"
#include "file/mid/MidiWriter.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <vector>

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::disk;
using namespace mpc::file::mid;

SCENARIO("A MidiFile can be written", "[file]")
{

    GIVEN("An Mpc with a Sequence")
    {

        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        auto sequencer = mpc.getSequencer();
        auto stateManager = sequencer->getStateManager();
        auto sequence = sequencer->getSequence(0);
        sequence->init(1);
        stateManager->drainQueue();
        auto track0 = sequence->getTrack(0);
        auto track1 = sequence->getTrack(1);
        track0->setUsed(true);
        track0->setDeviceIndex(2);

        EventState eventState;
        eventState.type = EventType::NoteOn;
        eventState.tick = 0;
        eventState.noteNumber = NoteNumber(37);
        eventState.velocity = MaxVelocity;
        eventState.duration = Duration(10);
        track0->insertEvent(eventState);
        stateManager->drainQueue();

        MidiWriter midiWriter(sequence.get());
        auto ostream = std::make_shared<std::ostringstream>();
        midiWriter.writeToOStream(ostream);

        sequence->init(1);
        stateManager->drainQueue();
        track0->removeEvents();
        stateManager->drainQueue();
        REQUIRE(track0->getEvents().empty());

        auto istream = std::make_shared<std::istringstream>(ostream->str());
        MidiReader midiReader(istream, sequence);
        midiReader.parseSequence(mpc);
        stateManager->drainQueue();

        REQUIRE(sequence->getTrack(0)->getEvents().size() == 1);
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(
            sequence->getTrack(0)->getEvents()[0]));
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(
                    sequence->getTrack(0)->getEvents()[0])
                    ->getNote() == 37);
        REQUIRE(sequence->getTrack(0)->getDeviceIndex() == 2);
        REQUIRE(sequence->getTrack(1)->getDeviceIndex() == 0);
    }
}

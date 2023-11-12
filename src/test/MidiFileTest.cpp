#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

#include <file/mid/MidiReader.hpp>
#include <file/mid/MidiWriter.hpp>

#include <string>
#include <vector>

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::disk;
using namespace mpc::midi;
using namespace mpc::file::mid;

SCENARIO("A MidiFile can be written", "[file]") {

	GIVEN("An Mpc with a Sequence") {

		mpc::Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        auto sequencer = mpc.getSequencer();
		auto sequence = sequencer->getSequence(0);
		sequence->init(1);
		auto track0 = sequence->getTrack(0);
		auto track1 = sequence->getTrack(1);
		track0->setUsed(true);
        track0->setDeviceIndex(2);

		auto noteEvent = track0->recordNoteEventSynced(0, 37, 127);
		noteEvent->setDuration(10);
		noteEvent->setVelocity(127);

		MidiWriter midiWriter(sequence.get());
        auto ostream = std::make_shared<std::ostringstream>();
        midiWriter.writeToOStream(ostream);
		
        sequence->init(1);
        track0->removeEvents();
        REQUIRE(track0->getEvents().empty());
        
        auto istream = std::make_shared<std::istringstream>(ostream->str());
        MidiReader midiReader(istream, sequence);
        midiReader.parseSequence(mpc);
        
        REQUIRE(sequence->getTrack(0)->getEvents().size() == 1);
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(sequence->getTrack(0)->getEvents()[0]));
        REQUIRE(std::dynamic_pointer_cast<NoteOnEvent>(sequence->getTrack(0)->getEvents()[0])->getNote() == 37);
        REQUIRE(sequence->getTrack(0)->getDeviceIndex() == 2);
        REQUIRE(sequence->getTrack(1)->getDeviceIndex() == 0);

    }
}

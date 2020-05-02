#include <catch2/catch.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>

#include <ui/UserDefaults.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <StartUp.hpp>

#include <midi/MidiFile.hpp>
#include <file/mid/MidiReader.hpp>
#include <file/mid/MidiWriter.hpp>


#include <file/File.hpp>
#include <file/FileUtil.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

#include <string>
#include <vector>

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::midi;
using namespace mpc::file::mid;
using namespace moduru::file;
using namespace std;

SCENARIO("A MidiFile can be written", "[file]") {

	GIVEN("An Mpc with a Sequence") {

		mpc::Mpc& mpc = mpc::Mpc::instance();
		mpc.init(44100, 1, 1);
		auto sequencer = mpc.getSequencer().lock();
		auto sequence = sequencer->getSequence(0).lock();
		sequence->init(1);
		auto track = sequence->getTrack(0).lock();
		track->setUsed(true);
		auto disk = mpc.getDisk().lock();

		auto noteEvent = track->addNoteEvent(0, 37).lock();
		noteEvent->setDuration(10);
		noteEvent->setVelocity(127);

		auto name = string("foo.mid");
		auto path = mpc::StartUp::storesPath + "MPC2000XL/" + name;
		auto fileToDelete = File(path, nullptr);
		fileToDelete.del();
		fileToDelete.close();

		auto midiWriter = MidiWriter(sequence.get());

		midiWriter.writeToFile(path);

		auto files = disk->getFiles();
		auto fileIterator = find_if(begin(files), end(files), [](MpcFile* f) { return f->getName().compare("FOO.MID") == 0; });

		REQUIRE(fileIterator != files.end());

		auto mpcFile = (*fileIterator);
		auto file = mpcFile->getFile().lock();

		REQUIRE(file->exists());
	
		auto midiReader = MidiReader(mpcFile, sequence);

		track = sequence->getTrack(0).lock();

		auto events = track->getNoteEvents();

		REQUIRE(events.size() == 1);
		
		auto event = events[0];
		
	}
}

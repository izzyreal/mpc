#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/Song.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/all/AllParser.hpp"
#include "file/all/AllSequence.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

void saveAndLoadTestAllFile(mpc::Mpc &mpc)
{
    AllParser allParser(mpc);
    auto bytes = allParser.getBytes();

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    sequencer->deleteAllSequences();

    stateManager->drainQueue();

    for (int i = 0; i < 20; i++)
    {
        sequencer->deleteSong(i);
    }

    stateManager->drainQueue();

    AllParser reparsed(mpc, bytes);
    AllLoader::loadEverythingFromAllParser(mpc, reparsed);
}

TEST_CASE("ALL file song", "[allfile]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    sequencer->getSequence(0)->init(1);
    stateManager->drainQueue();
    sequencer->getSequence(1)->init(1);
    stateManager->drainQueue();

    auto song = sequencer->getSong(0);

    assert(song->getStepCount() == 0);

    song->setUsed(true);
    song->setName("TestSong");
    song->insertStep(mpc::SongStepIndex(0));
    song->insertStep(mpc::SongStepIndex(1));
    song->setStepSequenceIndex(mpc::SongStepIndex(0), mpc::SequenceIndex(0));
    song->setStepRepetitionCount(mpc::SongStepIndex(0), 1);
    song->setStepSequenceIndex(mpc::SongStepIndex(1), mpc::SequenceIndex(1));
    song->setStepRepetitionCount(mpc::SongStepIndex(1), 2);

    stateManager->drainQueue();

    saveAndLoadTestAllFile(mpc);

    stateManager->drainQueue();

    song = sequencer->getSong(0);
    REQUIRE(song->getName() == "TestSong");
    REQUIRE(song->getStepCount() == 2);
    REQUIRE(song->getStep(mpc::SongStepIndex(0)).sequenceIndex == 0);
    REQUIRE(song->getStep(mpc::SongStepIndex(0)).repetitionCount == 1);
    REQUIRE(song->getStep(mpc::SongStepIndex(1)).sequenceIndex == 1);
    REQUIRE(song->getStep(mpc::SongStepIndex(1)).repetitionCount == 2);
}

TEST_CASE("ALL file save does not crash with sysex event", "[allfile]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSequence(0);
    seq->init(1);
    stateManager->drainQueue();

    EventData eventData;
    eventData.type = EventType::SystemExclusive;
    eventData.tick = 0;
    eventData.sysExByteA = 0x12;
    eventData.sysExByteB = 0x34;

    seq->getTrack(0)->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    AllParser allParser(mpc);
    REQUIRE_NOTHROW(allParser.getBytes());

    auto bytes = allParser.getBytes();
    AllParser reparsed(mpc, bytes);
    auto allSequences = reparsed.getAllSequences();
    REQUIRE_FALSE(allSequences.empty());
    REQUIRE(allSequences[0]->getEventAmount() == 1);
    REQUIRE(allSequences[0]->allEvents[0].type == EventType::SystemExclusive);
    REQUIRE(allSequences[0]->allEvents[0].sysExByteA == 0x12);
    REQUIRE(allSequences[0]->allEvents[0].sysExByteB == 0x34);
}

TEST_CASE("ALL file track is on, used and transmits program changes",
          "[allfile]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSequence(0);
    seq->init(1);
    seq->getTrack(42)->setTransmitProgramChangesEnabled(true);
    seq->getTrack(43)->setTransmitProgramChangesEnabled(false);
    seq->getTrack(60)->setOn(true);
    seq->getTrack(61)->setOn(false);
    seq->getTrack(62)->setUsedIfCurrentlyUnused();
    seq->getTrack(62)->setOn(true);
    seq->getTrack(63)->setUsedIfCurrentlyUnused();
    seq->getTrack(63)->setOn(false);

    stateManager->drainQueue();

    saveAndLoadTestAllFile(mpc);

    stateManager->drainQueue();

    assert(sequencer->getUsedSequenceCount() == 1);

    auto seq1 = sequencer->getSelectedSequence();
    REQUIRE(seq1->getTrack(42)->isTransmitProgramChangesEnabled());
    REQUIRE(!seq1->getTrack(43)->isTransmitProgramChangesEnabled());
    REQUIRE(seq1->getTrack(60)->isUsed());
    REQUIRE(seq1->getTrack(60)->isOn());
    REQUIRE(seq1->getTrack(61)->isUsed());
    REQUIRE(!seq1->getTrack(61)->isOn());
    REQUIRE(seq1->getTrack(62)->isUsed());
    REQUIRE(seq1->getTrack(62)->isOn());
    REQUIRE(seq1->getTrack(63)->isUsed());
    REQUIRE(!seq1->getTrack(63)->isOn());
}

TEST_CASE("ALL file note event", "[allfile]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSequence(0);
    seq->init(1);
    sequencer->getStateManager()->drainQueue();
    auto tr = seq->getTrack(63);

    auto e = tr->recordNoteEventNonLive(0, mpc::NoteNumber(60),
                                        mpc::Velocity(127), 0);
    tr->finalizeNoteEventNonLive(e, mpc::Duration(1600));
    stateManager->drainQueue();
    tr->getNoteEvents().front()->setVariationType(mpc::NoteVariationTypeFilter);
    stateManager->drainQueue();
    tr->getNoteEvents().front()->setVariationValue(mpc::NoteVariationValue(20));
    stateManager->drainQueue();

    saveAndLoadTestAllFile(mpc);

    stateManager->drainQueue();

    assert(sequencer->getUsedSequenceCount() == 1);

    auto seq1 = sequencer->getSelectedSequence();
    auto tr1 = seq1->getTrack(63);
    stateManager->drainQueue();
    auto event1 = tr1->getEvent(0);
    REQUIRE(event1->getTypeName() == "note-on");
    auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(event1);
    REQUIRE(noteEvent->getNote() == 60);
    REQUIRE(noteEvent->getVelocity() == 127);
    REQUIRE(noteEvent->getTrack() == tr->getIndex());
    REQUIRE(noteEvent->getDuration() == 1600);
    REQUIRE(noteEvent->getTick() == 0);
    REQUIRE(noteEvent->getVariationType() == 3);
    REQUIRE(noteEvent->getVariationValue() == 20);
}

TEST_CASE("ALL file program change event", "[allfile]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSequence(0);
    seq->init(1);
    stateManager->drainQueue();

    EventData eventData;
    eventData.type = EventType::ProgramChange;
    eventData.tick = 96;
    eventData.programChangeProgramIndex = mpc::ProgramIndex(7);

    seq->getTrack(63)->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    saveAndLoadTestAllFile(mpc);

    stateManager->drainQueue();

    REQUIRE(sequencer->getUsedSequenceCount() == 1);

    auto seq1 = sequencer->getSelectedSequence();
    REQUIRE(seq1);
    auto tr1 = seq1->getTrack(63);
    REQUIRE(tr1);
    auto event1 = tr1->getEvent(0);
    REQUIRE(event1);
    REQUIRE(event1->getTypeName() == "program-change");
    auto programChangeEvent = std::dynamic_pointer_cast<ProgramChangeEvent>(event1);
    REQUIRE(programChangeEvent);
    REQUIRE(programChangeEvent->getTick() == 96);
    REQUIRE(programChangeEvent->getProgram() == 7);
}

TEST_CASE("ALL file track device is remembered and restored", "[allfile]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    auto seq = sequencer->getSequence(0);
    seq->init(1);
    seq->getTrack(60)->setDeviceIndex(1);
    seq->getTrack(61)->setDeviceIndex(2);
    seq->getTrack(62)->setDeviceIndex(3);
    seq->getTrack(63)->setDeviceIndex(4);

    stateManager->drainQueue();

    saveAndLoadTestAllFile(mpc);

    stateManager->drainQueue();

    assert(sequencer->getUsedSequenceCount() == 1);

    auto seq1 = sequencer->getSelectedSequence();

    REQUIRE(seq1->getTrack(60)->getDeviceIndex() == 1);
    REQUIRE(seq1->getTrack(61)->getDeviceIndex() == 2);
    REQUIRE(seq1->getTrack(62)->getDeviceIndex() == 3);
    REQUIRE(seq1->getTrack(63)->getDeviceIndex() == 4);
}

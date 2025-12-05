#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/all/AllParser.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

// Since we're writing the ALL file to the same directory as a production
// instance of VMPC2000XL would, we choose a strange name that is unlikely to
// exist.
const std::string filename = "SHS4CABHNAXJ9LQ.ALL";

void deleteTestAllFile(const std::shared_ptr<AbstractDisk> &disk)
{
    disk->initFiles();

    for (int i = 0; i < disk->getAllFiles().size(); i++)
    {
        if (disk->getFileName(i) == filename)
        {
            (void)disk->getFile(i)->del();
            disk->flush();
            disk->initFiles();
            break;
        }
    }
}

void saveAndLoadTestAllFile(mpc::Mpc &mpc)
{
    const auto disk = mpc.getDisk();
    const auto f = disk->newFile(filename);

    AllParser allParser(mpc);
    auto bytes = allParser.getBytes();
    f->setFileData(bytes);

    disk->flush();
    disk->initFiles();

    const auto sequencer = mpc.getSequencer();

    sequencer->purgeAllSequences();

    for (int i = 0; i < 20; i++)
    {
        sequencer->deleteSong(i);
    }

    AllLoader::loadEverythingFromFile(mpc, disk->getFile(filename).get());
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
    song->getStep(mpc::SongStepIndex(0))
        .lock()
        ->setSequence(mpc::SequenceIndex(0));
    song->getStep(mpc::SongStepIndex(0)).lock()->setRepeats(1);
    song->getStep(mpc::SongStepIndex(1))
        .lock()
        ->setSequence(mpc::SequenceIndex(1));
    song->getStep(mpc::SongStepIndex(1)).lock()->setRepeats(2);

    auto disk = mpc.getDisk();

    deleteTestAllFile(disk);

    saveAndLoadTestAllFile(mpc);

    song = sequencer->getSong(0);
    REQUIRE(song->getName() == "TestSong");
    REQUIRE(song->getStepCount() == 2);
    REQUIRE(song->getStep(mpc::SongStepIndex(0)).lock()->getSequenceIndex() == 0);
    REQUIRE(song->getStep(mpc::SongStepIndex(0)).lock()->getRepeats() == 1);
    REQUIRE(song->getStep(mpc::SongStepIndex(1)).lock()->getSequenceIndex() == 1);
    REQUIRE(song->getStep(mpc::SongStepIndex(1)).lock()->getRepeats() == 2);
}

TEST_CASE("ALL file track is on and used", "[allfile]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSequence(0);
    seq->init(1);
    seq->getTrack(60)->setUsed(false);
    seq->getTrack(60)->setOn(true);
    seq->getTrack(61)->setUsed(false);
    seq->getTrack(61)->setOn(false);
    seq->getTrack(62)->setUsed(true);
    seq->getTrack(62)->setOn(true);
    seq->getTrack(63)->setUsed(true);
    seq->getTrack(63)->setOn(false);

    stateManager->drainQueue();

    auto disk = mpc.getDisk();

    deleteTestAllFile(disk);

    saveAndLoadTestAllFile(mpc);

    assert(sequencer->getUsedSequenceCount() == 1);

    auto seq1 = sequencer->getSelectedSequence();
    REQUIRE(!seq1->getTrack(60)->isUsed());
    REQUIRE(seq1->getTrack(60)->isOn());
    REQUIRE(!seq1->getTrack(61)->isUsed());
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

    auto disk = mpc.getDisk();

    deleteTestAllFile(disk);

    saveAndLoadTestAllFile(mpc);

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

    auto disk = mpc.getDisk();

    deleteTestAllFile(disk);

    saveAndLoadTestAllFile(mpc);

    assert(sequencer->getUsedSequenceCount() == 1);

    auto seq1 = sequencer->getSelectedSequence();

    REQUIRE(seq1->getTrack(60)->getDeviceIndex() == 1);
    REQUIRE(seq1->getTrack(61)->getDeviceIndex() == 2);
    REQUIRE(seq1->getTrack(62)->getDeviceIndex() == 3);
    REQUIRE(seq1->getTrack(63)->getDeviceIndex() == 4);
}

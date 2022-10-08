#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/all/AllNoteEvent.hpp"
#include "file/all/AllParser.hpp"

using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

// Since we're writing the ALL file to the same directory as a production instance
// of VMPC2000XL would, we choose a strange name that is unlikely to exist.
const std::string filename = "SHS4CABHNAXJ9LQ.ALL";

void deleteTestAllFile(const std::shared_ptr<mpc::disk::AbstractDisk>& disk)
{
    disk->initFiles();

    for (int i = 0; i < disk->getAllFiles().size(); i ++)
    {
        if (disk->getFileName(i) == filename)
        {
            disk->getFile(i)->del();
            disk->flush();
            disk->initFiles();
            break;
        }
    }
}

void saveAndLoadTestAllFile(mpc::Mpc& mpc)
{
    auto disk = mpc.getDisk().lock();
    auto f = disk->newFile(filename);

    mpc::file::all::AllParser allParser(mpc, f->getNameWithoutExtension());
    auto bytes = allParser.getBytes();
    f->setFileData(bytes);

    disk->flush();
    disk->initFiles();

    mpc.getSequencer().lock()->purgeAllSequences();

    for (int i = 0; i < 20; i++)
    {
        mpc.getSequencer().lock()->deleteSong(i);
    }

    AllLoader::loadEverythingFromFile(mpc, disk->getFile(filename).get());
}

TEST_CASE("ALL file song", "[allfile]")
{
    mpc::Mpc mpc;
    mpc.init(44100, 1, 1);
    auto sequencer = mpc.getSequencer().lock();

    sequencer->getSequence(0).lock()->init(1);
    sequencer->getSequence(1).lock()->init(1);

    auto song = sequencer->getSong(0).lock();

    assert(song->getStepCount() == 0);

    song->setUsed(true);
    song->setName("TestSong");
    song->insertStep(0);
    song->insertStep(1);
    song->getStep(0).lock()->setSequence(0);
    song->getStep(0).lock()->setRepeats(1);
    song->getStep(1).lock()->setSequence(1);
    song->getStep(1).lock()->setRepeats(2);

    auto disk = mpc.getDisk().lock();

    deleteTestAllFile(disk);

    saveAndLoadTestAllFile(mpc);

    song = sequencer->getSong(0).lock();
    REQUIRE(song->getName() == "TestSong");
    REQUIRE(song->getStepCount() == 2);
    REQUIRE(song->getStep(0).lock()->getSequence() == 0);
    REQUIRE(song->getStep(0).lock()->getRepeats() == 1);
    REQUIRE(song->getStep(1).lock()->getSequence() == 1);
    REQUIRE(song->getStep(1).lock()->getRepeats() == 2);
}

TEST_CASE("ALL file track is on and used", "[allfile]")
{
    mpc::Mpc mpc;
    mpc.init(44100, 1, 1);
    auto seq = mpc.getSequencer().lock()->getSequence(0).lock();
    seq->init(1);
    seq->getTrack(60).lock()->setUsed(false);
    seq->getTrack(60).lock()->setOn(true);
    seq->getTrack(61).lock()->setUsed(false);
    seq->getTrack(61).lock()->setOn(false);
    seq->getTrack(62).lock()->setUsed(true);
    seq->getTrack(62).lock()->setOn(true);
    seq->getTrack(63).lock()->setUsed(true);
    seq->getTrack(63).lock()->setOn(false);
    auto disk = mpc.getDisk().lock();

    deleteTestAllFile(disk);

    saveAndLoadTestAllFile(mpc);

    REQUIRE(mpc.getSequencer().lock()->getUsedSequenceCount() == 1);
    auto seq1 = mpc.getSequencer().lock()->getActiveSequence().lock();
    REQUIRE(!seq1->getTrack(60).lock()->isUsed());
    REQUIRE(seq1->getTrack(60).lock()->isOn());
    REQUIRE(!seq1->getTrack(61).lock()->isUsed());
    REQUIRE(!seq1->getTrack(61).lock()->isOn());
    REQUIRE(seq1->getTrack(62).lock()->isUsed());
    REQUIRE(seq1->getTrack(62).lock()->isOn());
    REQUIRE(seq1->getTrack(63).lock()->isUsed());
    REQUIRE(!seq1->getTrack(63).lock()->isOn());
}

TEST_CASE("ALL file note event", "[allfile]")
{
    mpc::Mpc mpc;
    mpc.init(44100, 1, 1);
    auto seq = mpc.getSequencer().lock()->getSequence(0).lock();
    seq->init(1);
    auto tr = seq->getTrack(63).lock();
    auto event = std::dynamic_pointer_cast<mpc::sequencer::NoteEvent>(tr->addEvent(0, "note").lock());
    event->setNote(0);
    event->setTrack(tr->getIndex());
    event->setVelocity(127);
    event->setDuration(1600);
    event->setTick(0);
    event->setVariationTypeNumber(3);
    event->setVariationValue(20);

    auto disk = mpc.getDisk().lock();

    deleteTestAllFile(disk);

    saveAndLoadTestAllFile(mpc);

    REQUIRE(mpc.getSequencer().lock()->getUsedSequenceCount() == 1);
    auto seq1 = mpc.getSequencer().lock()->getActiveSequence().lock();
    auto tr1 = seq1->getTrack(63).lock();
    auto event1 = tr1->getEvent(0).lock();
    REQUIRE(event1->getTypeName() == "note");
    auto noteEvent = std::dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event1);
    REQUIRE(noteEvent->getNote() == 0);
    REQUIRE(noteEvent->getVelocity() == 127);
    REQUIRE(noteEvent->getTrack() == tr->getIndex());
    REQUIRE(noteEvent->getDuration() == 1600);
    REQUIRE(noteEvent->getTick() == 0);
    REQUIRE(noteEvent->getVariationType() == 3);
    REQUIRE(noteEvent->getVariationValue() == 20);
}
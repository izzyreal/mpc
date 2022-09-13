#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/all/AllNoteEvent.hpp"
#include "file/all/AllParser.hpp"

using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

TEST_CASE("ALL file", "[allfile]")
{
    // Since we're writing the ALL file to the same directory as a production instance
    // of VMPC2000XL would, we choose a strange name that is unlikely to exist.
    const std::string filename = "SHS4CABHNAXJ9LQ.ALL";
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

    auto f = disk->newFile(filename);

    mpc::file::all::AllParser allParser(mpc, f->getNameWithoutExtension());
    auto bytes = allParser.getBytes();
    f->setFileData(bytes);

    disk->flush();
    disk->initFiles();

    mpc.getSequencer().lock()->purgeAllSequences();
    AllLoader::loadEverythingFromFile(mpc, disk->getFile(filename).get());

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
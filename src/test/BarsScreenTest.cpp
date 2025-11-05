#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

TEST_CASE("BARS1", "[bars-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getActiveSequence();
    seq->init(0);
    seq->setTimeSignature(0, 4, 4);
    auto tr = seq->getTrack(0);

    int tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        auto noteEvent = std::make_shared<NoteOnEvent>(35 + i);
        noteEvent->setVelocity(127);
        noteEvent->setDuration(42);
        noteEvent->setTick(tickPos);
        tr->insertEventWhileRetainingSort(noteEvent);
        tickPos += 24;
    }

    auto barsScreen = mpc.screens->get<ScreenId::BarsScreen>();
    int toSeqIndex = 1;
    int firstBar = 0;
    int lastBar = 0;
    int copies = 1;
    int afterBar = 0;

    barsScreen->copyBars(toSeqIndex, firstBar, lastBar, copies, afterBar);

    auto toSeq = mpc.getSequencer()->getSequence(toSeqIndex);

    REQUIRE(toSeq->isUsed());

    auto expectedBarCount = ((lastBar - firstBar) + 1) * copies;

    REQUIRE(toSeq->getLastBarIndex() + 1 == expectedBarCount);

    for (int i = 0; i < 8; i++)
    {
        REQUIRE(toSeq->getTrack(0)->getEvent(i)->getTick() == i * 24);
    }
}

TEST_CASE("BARS2", "[bars-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getActiveSequence();
    seq->init(0);
    seq->setTimeSignature(0, 4, 4);
    auto tr = seq->getTrack(0);

    int tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        auto noteEvent = std::make_shared<NoteOnEvent>(35 + i);
        noteEvent->setVelocity(127);
        noteEvent->setDuration(42);
        noteEvent->setTick(tickPos);
        tr->insertEventWhileRetainingSort(noteEvent);
        tickPos += 24;
    }

    auto barsScreen = mpc.screens->get<ScreenId::BarsScreen>();
    int toSeqIndex = 1;
    int firstBar = 0;
    int lastBar = 0;
    int copies = 1;
    int afterBar = 0;

    auto toSeq = mpc.getSequencer()->getSequence(toSeqIndex);

    toSeq->init(0);
    toSeq->setTimeSignature(0, 4, 4);

    for (int i = 0; i < 16; i++)
    {
        auto e = std::make_shared<NoteOnEvent>(35 + i);
        toSeq->getTrack(0)->insertEventWhileRetainingSort(e, i * 24);
    }

    auto expectedBarCount = ((lastBar - firstBar) + 1) * copies;
    expectedBarCount += toSeq->getLastBarIndex() + 1;

    barsScreen->copyBars(toSeqIndex, firstBar, lastBar, copies, afterBar);

    REQUIRE(toSeq->getLastBarIndex() + 1 == expectedBarCount);
    REQUIRE(toSeq->getTrack(0)->getEvents().size() == 24);

    for (int eventIndex = 0; eventIndex < 8; eventIndex++)
    {
        auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(
            toSeq->getTrack(0)->getEvent(eventIndex));

        REQUIRE(noteEvent->getTick() == eventIndex * 24);
        REQUIRE(noteEvent->getNote() == eventIndex + 35);
    }
}

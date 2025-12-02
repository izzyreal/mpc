#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"

#include "sequencer/EventRef.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

TEST_CASE("BARS1", "[bars-screen]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    auto tr = seq->getTrack(0);

    int tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = tickPos;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        tr->acquireAndInsertEvent(eventData);
        tickPos += 24;
    }

    stateManager->drainQueue();

    REQUIRE(tr->getEvents().size() == 8);

    auto barsScreen = mpc.screens->get<ScreenId::BarsScreen>();
    int toSeqIndex = 1;
    int firstBar = 0;
    int lastBar = 0;
    int copies = 1;
    int afterBar = 0;

    barsScreen->copyBars(toSeqIndex, firstBar, lastBar, copies, afterBar);

    auto toSeq = mpc.getSequencer()->getSequence(toSeqIndex);
    stateManager->drainQueue();

    REQUIRE(toSeq->isUsed());

    auto expectedBarCount = (lastBar - firstBar + 1) * copies;

    REQUIRE(toSeq->getBarCount() == expectedBarCount);

    for (int i = 0; i < 8; i++)
    {
        REQUIRE(toSeq->getTrack(0)->getEvent(i)->getTick() == i * 24);
    }
}

TEST_CASE("BARS2", "[bars-screen]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = mpc.getSequencer()->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();

    int tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = tickPos;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        seq->getTrack(0)->acquireAndInsertEvent(eventData);
        tickPos += 24;
    }

    stateManager->drainQueue();

    REQUIRE(seq->getTrack(0)->getEvents().size() == 8);

    auto barsScreen = mpc.screens->get<ScreenId::BarsScreen>();
    int toSeqIndex = 1;
    int firstBar = 0;
    int lastBar = 0;
    int copies = 1;
    int afterBar = 0;

    auto toSeq = mpc.getSequencer()->getSequence(toSeqIndex);

    toSeq->init(0);
    stateManager->drainQueue();
    toSeq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();

    for (int i = 0; i < 16; i++)
    {
        EventData eventData;
        eventData.type = EventType::NoteOn;
        eventData.tick = i * 24;
        eventData.noteNumber = NoteNumber(35 + i);
        eventData.velocity = MaxVelocity;
        eventData.duration = Duration(42);
        toSeq->getTrack(0)->acquireAndInsertEvent(eventData);
    }

    stateManager->drainQueue();

    auto expectedBarCount = (lastBar - firstBar + 1) * copies;
    expectedBarCount += toSeq->getBarCount();

    barsScreen->copyBars(toSeqIndex, firstBar, lastBar, copies, afterBar);
    stateManager->drainQueue();
    stateManager->drainQueue();

    REQUIRE(toSeq->getBarCount() == expectedBarCount);
    REQUIRE(toSeq->getTrack(0)->getEvents().size() == 24);

    for (int eventIndex = 0; eventIndex < 8; eventIndex++)
    {
        auto noteEvent = std::dynamic_pointer_cast<NoteOnEvent>(
            toSeq->getTrack(0)->getEvent(eventIndex));

        REQUIRE(noteEvent->getTick() == eventIndex * 24);
        REQUIRE(noteEvent->getNote() == eventIndex + 35);
    }
}

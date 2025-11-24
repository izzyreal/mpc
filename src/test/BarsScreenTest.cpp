#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"

#include "sequencer/Event.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/SequenceStateManager.hpp"
#include "sequencer/NonRtSequencerStateManager.hpp"

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

TEST_CASE("BARS1", "[bars-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto seq = mpc.getSequencer()->getSelectedSequence();
    seq->init(0);
    seq->getStateManager()->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    auto tr = seq->getTrack(0);

    int tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        EventState eventState;
        eventState.type = EventType::NoteOn;
        eventState.tick = tickPos;
        eventState.noteNumber = NoteNumber(35 + i);
        eventState.velocity = MaxVelocity;
        eventState.duration = Duration(42);
        tr->insertEvent(eventState);
        tickPos += 24;
    }

    tr->getEventStateManager()->drainQueue();

    REQUIRE(tr->getEvents().size() == 8);

    auto barsScreen = mpc.screens->get<ScreenId::BarsScreen>();
    int toSeqIndex = 1;
    int firstBar = 0;
    int lastBar = 0;
    int copies = 1;
    int afterBar = 0;

    barsScreen->copyBars(toSeqIndex, firstBar, lastBar, copies, afterBar);

    auto toSeq = mpc.getSequencer()->getSequence(toSeqIndex);
    toSeq->getStateManager()->drainQueue();

    for (auto &t : toSeq->getTracks())
    {
        t->getEventStateManager()->drainQueue();
    }

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
    auto seq = mpc.getSequencer()->getSelectedSequence();
    seq->init(0);
    seq->getStateManager()->drainQueue();
    seq->setTimeSignature(0, 4, 4);

    int tickPos = 0;

    for (int i = 0; i < 8; i++)
    {
        EventState eventState;
        eventState.type = EventType::NoteOn;
        eventState.tick = tickPos;
        eventState.noteNumber = NoteNumber(35 + i);
        eventState.velocity = MaxVelocity;
        eventState.duration = Duration(42);
        seq->getTrack(0)->insertEvent(eventState);
        tickPos += 24;
    }

    seq->getTrack(0)->getEventStateManager()->drainQueue();

    REQUIRE(seq->getTrack(0)->getEvents().size() == 8);

    auto barsScreen = mpc.screens->get<ScreenId::BarsScreen>();
    int toSeqIndex = 1;
    int firstBar = 0;
    int lastBar = 0;
    int copies = 1;
    int afterBar = 0;

    auto toSeq = mpc.getSequencer()->getSequence(toSeqIndex);

    toSeq->init(0);
    toSeq->getStateManager()->drainQueue();
    toSeq->setTimeSignature(0, 4, 4);

    for (int i = 0; i < 16; i++)
    {
        EventState eventState;
        eventState.type = EventType::NoteOn;
        eventState.tick = i * 24;
        eventState.noteNumber = NoteNumber(35 + i);
        eventState.velocity = MaxVelocity;
        eventState.duration = Duration(42);
        toSeq->getTrack(0)->insertEvent(eventState);
    }

    toSeq->getTrack(0)->getEventStateManager()->drainQueue();

    auto expectedBarCount = ((lastBar - firstBar) + 1) * copies;
    expectedBarCount += toSeq->getLastBarIndex() + 1;

    barsScreen->copyBars(toSeqIndex, firstBar, lastBar, copies, afterBar);
    toSeq->getStateManager()->drainQueue();
    toSeq->getTrack(0)->getEventStateManager()->drainQueue();

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

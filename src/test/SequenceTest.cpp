#include "sequencer/Sequence.hpp"
#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/BarsScreen.hpp"
#include "sequencer/EventRef.hpp"
#include "sequencer/SequenceMessage.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <atomic>
#include <thread>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

namespace
{
void insertTestNote(
    const std::shared_ptr<Track> &track,
    const mpc::Tick tick,
    const int noteNumber)
{
    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = tick;
    eventData.noteNumber = mpc::NoteNumber(noteNumber);
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(24);
    track->acquireAndInsertEvent(eventData);
}

void enqueueBulkNote(
    const std::shared_ptr<SequencerStateManager> &stateManager,
    const mpc::SequenceIndex sequenceIndex,
    const mpc::TrackIndex trackIndex,
    const mpc::Tick tick,
    const int noteNumber)
{
    auto &trackEvents = stateManager->trackEventsSnapshots[sequenceIndex];
    trackEvents.clear();

    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = tick;
    eventData.noteNumber = mpc::NoteNumber(noteNumber);
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(24);
    trackEvents[trackIndex].push_back(eventData);

    UpdateSequenceEvents update{sequenceIndex};
    update.trackSnapshots = &trackEvents;
    stateManager->enqueue(update);
}
}

TEST_CASE("Insert bar and move event forward", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    stateManager->drainQueue();
    auto tr = seq->getTrack(0);

    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 0;
    eventData.noteNumber = mpc::MinDrumNoteNumber;
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(42);

    tr->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    seq->insertBars(1, mpc::BarIndex(0));
    stateManager->drainQueue();
    stateManager->drainQueue();
    REQUIRE(tr->getEvent(0)->getTick() == 384);
}

TEST_CASE("SeqUtil setBar keeps beat position in 2/4", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();

    seq->init(7); // 8 bars.
    stateManager->drainQueue();
    seq->setTimeSignature(0, seq->getLastBarIndex(), 2, 4);
    stateManager->drainQueue();

    auto position = 0;
    position = SeqUtil::setBar(1, seq.get(), position);
    REQUIRE(position == 192);
    position = SeqUtil::setBar(2, seq.get(), position);
    REQUIRE(position == 384);
    position = SeqUtil::setBar(3, seq.get(), position);
    REQUIRE(position == 576);
}

TEST_CASE("SeqUtil beat and clock are local to each bar", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();

    seq->init(1); // 2 bars.
    stateManager->drainQueue();
    seq->setTimeSignature(0, 4, 4);
    seq->setTimeSignature(1, 3, 4);
    stateManager->drainQueue();

    // Start of bar 2 in 3/4 must be beat 1, clock 00 on screen terms (0, 0
    // internally), regardless of bar 1 length.
    REQUIRE(SeqUtil::getBar(seq.get(), 384) == 1);
    REQUIRE(SeqUtil::getBeat(seq.get(), 384) == 0);
    REQUIRE(SeqUtil::getClock(seq.get(), 384) == 0);

    REQUIRE(SeqUtil::getBeat(seq.get(), 384 + 96) == 1);
    REQUIRE(SeqUtil::getBeat(seq.get(), 384 + 192) == 2);
    REQUIRE(SeqUtil::getClock(seq.get(), 384 + 192 + 23) == 23);
}

TEST_CASE("UpdateSequenceTracks preserves existing events", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    auto track = seq->getTrack(0);
    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 12;
    eventData.noteNumber = mpc::MinDrumNoteNumber;
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(24);
    track->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    auto &trackStates =
        stateManager->trackStatesSnapshots[seq->getSequenceIndex()];
    trackStates = SequenceTrackStatesSnapshot();
    trackStates[0].name = "BulkMeta";
    trackStates[0].used = true;
    trackStates[0].busType = BusType::MIDI;
    trackStates[0].deviceIndex = 3;

    UpdateSequenceTracks update{seq->getSequenceIndex()};
    update.trackStates = &trackStates;
    stateManager->enqueue(update);
    stateManager->drainQueue();

    REQUIRE(track->getEvents().size() == 1);
    REQUIRE(track->getEvent(0)->getTick() == 12);
    REQUIRE(track->getName() == "BulkMeta");
    REQUIRE(track->getBusType() == BusType::MIDI);
    REQUIRE(track->getDeviceIndex() == 3);
}

TEST_CASE("UpdateSequenceEvents appends imported events", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    const auto track = seq->getTrack(0);
    insertTestNote(track, 12, 35);
    stateManager->drainQueue();

    enqueueBulkNote(
        stateManager, seq->getSequenceIndex(), track->getIndex(), 48, 36);
    stateManager->drainQueue();

    REQUIRE(track->getEvents().size() == 2);
    REQUIRE(track->getEvent(0)->getTick() == 12);
    REQUIRE(track->getEvent(1)->getTick() == 48);
}

TEST_CASE("CopySequence copies event lists without sharing handles", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto source = sequencer->getSequence(0);
    const auto dest = sequencer->getSequence(1);
    source->init(0);
    dest->init(0);
    stateManager->drainQueue();

    const auto sourceTrack = source->getTrack(0);
    const auto destTrack = dest->getTrack(0);
    insertTestNote(sourceTrack, 24, 35);
    stateManager->drainQueue();

    sequencer->copySequence(mpc::SequenceIndex(0), mpc::SequenceIndex(1));
    stateManager->drainQueue();

    REQUIRE(destTrack->getEvents().size() == 1);
    REQUIRE(destTrack->getEvent(0)->getTick() == 24);
    REQUIRE(destTrack->getEvent(0)->handle != sourceTrack->getEvent(0)->handle);
}

TEST_CASE("DeleteSequence clears event lists", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSequence(0);
    seq->init(0);
    stateManager->drainQueue();

    const auto track = seq->getTrack(0);
    insertTestNote(track, 24, 35);
    stateManager->drainQueue();
    REQUIRE(track->getEvents().size() == 1);

    sequencer->deleteSequence(0);
    stateManager->drainQueue();

    REQUIRE(track->getEvents().empty());
}

TEST_CASE("EraseEvents removes matching events", "[sequence]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    const auto track = seq->getTrack(0);
    insertTestNote(track, 12, 35);
    insertTestNote(track, 48, 36);
    stateManager->drainQueue();

    stateManager->enqueue(EraseEvents{
        seq->getSequenceIndex(), track->getIndex(), mpc::Tick(0),
        mpc::Tick(24), 0, 0, NoteRange::All()});
    stateManager->drainQueue();
    stateManager->drainQueue();

    REQUIRE(track->getEvents().size() == 1);
    REQUIRE(track->getEvent(0)->getTick() == 48);
}

TEST_CASE("Track event readers tolerate concurrent sequence mutations",
          "[sequence][sequencer-multithread]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    const auto stateManager = sequencer->getStateManager();
    const auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    const auto track = seq->getTrack(0);
    insertTestNote(track, 12, 35);
    insertTestNote(track, 48, 36);
    stateManager->drainQueue();

    std::atomic<bool> stop{false};
    std::atomic<int> snapshotsRead{0};

    std::thread reader(
        [&]
        {
            while (!stop.load())
            {
                for (const auto &event : track->getEvents())
                {
                    REQUIRE(event != nullptr);
                    (void)event->getTick();
                }
                ++snapshotsRead;
            }
        });

    for (int i = 0; i < 50; ++i)
    {
        enqueueBulkNote(
            stateManager, seq->getSequenceIndex(), track->getIndex(),
            mpc::Tick((i % 8) * 12), 37 + (i % 8));
        stateManager->drainQueue();

        auto &trackStates =
            stateManager->trackStatesSnapshots[seq->getSequenceIndex()];
        trackStates = SequenceTrackStatesSnapshot();
        trackStates[0].name = "Stress";
        trackStates[0].used = true;
        UpdateSequenceTracks updateTracks{seq->getSequenceIndex()};
        updateTracks.trackStates = &trackStates;
        stateManager->enqueue(updateTracks);
        stateManager->drainQueue();

        stateManager->enqueue(EraseEvents{
            seq->getSequenceIndex(), track->getIndex(), mpc::Tick(0),
            mpc::Tick(24), 0, 0, NoteRange::All()});
        stateManager->drainQueue();
        stateManager->drainQueue();

        sequencer->copySequence(seq->getSequenceIndex(), mpc::UndoSequenceIndex);
        stateManager->drainQueue();
    }

    stop.store(true);
    reader.join();

    REQUIRE(snapshotsRead.load() > 0);
}

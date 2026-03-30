#include <catch2/catch_test_macros.hpp>
#include "sequencer/Transport.hpp"

#include "TestMpc.hpp"
#include "client/event/ClientEvent.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "sequencer/Clock.hpp"

#include "command/TriggerLocalNoteOffCommand.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"

#include "engine/EngineHost.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "performance/PerformanceManager.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/EventData.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/SequenceStateView.hpp"

#include "hardware/Component.hpp"
#include "sequencer/EventRef.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "audiomidi/MidiOutput.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::client::event;
using namespace mpc::hardware;
using namespace mpc::lcdgui;

namespace
{
constexpr int kPlaybackBufferSize = 512;
constexpr int kPlaybackSampleRate = 44100;

void prepareMidiPlaybackTrack(const std::shared_ptr<mpc::sequencer::Track> &track,
                              const mpc::Tick tick,
                              const int noteNumber)
{
    track->setBusType(mpc::sequencer::BusType::MIDI);
    track->setDeviceIndex(1);
    auto *note = track->recordNoteEventNonLive(
        tick, mpc::NoteNumber(noteNumber), mpc::Velocity(100));
    track->finalizeNoteEventNonLive(note, mpc::Duration(1));
}

void processPlaybackBuffers(mpc::Mpc &mpc, const int bufferCount)
{
    auto server = mpc.getEngineHost()->getAudioServer();
    auto sequencer = mpc.getSequencer();

    server->resizeBuffers(kPlaybackBufferSize);
    server->setSampleRate(kPlaybackSampleRate);
    server->start();

    for (int i = 0; i < bufferCount; ++i)
    {
        mpc.getEngineHost()->prepareProcessBlock(kPlaybackBufferSize);
        mpc.getClock()->processBufferInternal(
            sequencer->getTransport()->getTempo(), kPlaybackSampleRate,
            kPlaybackBufferSize,
            sequencer->getTransport()->getPlayStartPositionQuarterNotes());
        server->work(nullptr, nullptr, kPlaybackBufferSize, {}, {}, {}, {});
    }
}

void processExternalPlaybackBuffer(mpc::Mpc &mpc,
                                   const double positionQuarterNotes,
                                   const int nFrames,
                                   const int64_t timeInSamples)
{
    auto server = mpc.getEngineHost()->getAudioServer();
    server->resizeBuffers(nFrames);
    server->setSampleRate(kPlaybackSampleRate);
    server->start();

    mpc.getEngineHost()->prepareProcessBlock(nFrames);
    mpc.getClock()->processBufferExternal(positionQuarterNotes, nFrames,
                                          kPlaybackSampleRate,
                                          mpc.getSequencer()->getTransport()->getTempo(),
                                          timeInSamples);
    mpc.getEngineHost()->getSequencerPlaybackEngine()->work(nFrames);
}

std::unordered_map<int, int> dequeueMidiNoteOnCounts(mpc::Mpc &mpc)
{
    std::unordered_map<int, int> result;
    std::vector<mpc::client::event::ClientMidiEvent> buffer(256);

    while (true)
    {
        const auto dequeued = mpc.getMidiOutput()->dequeue(buffer);

        if (dequeued == 0)
        {
            break;
        }

        for (int i = 0; i < dequeued; ++i)
        {
            const auto &event = buffer[i];

            if (event.getMessageType() ==
                mpc::client::event::ClientMidiEvent::NOTE_ON)
            {
                result[event.getNoteNumber()]++;
            }
        }
    }

    return result;
}

void configureSecondSequence(mpc::Mpc &mpc, const int sequenceIndex)
{
    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SecondSeqScreen);
    auto secondSeqScreen = mpc.screens->get<ScreenId::SecondSeqScreen>();
    secondSeqScreen->turnWheel(sequenceIndex - secondSeqScreen->getSq());
    mpc.getSequencer()->setSecondSequenceEnabled(true);
    mpc.getSequencer()->getStateManager()->drainQueue();
}
}

TEST_CASE("Next step, previous step", "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    auto pos = [&]
    {
        return sequencer->getTransport()->getTickPosition();
    };

    auto seq = sequencer->getSequence(0);
    seq->init(1);
    stateManager->drainQueue();
    seq->setTimeSignature(0, 1, 32);
    stateManager->drainQueue();
    seq->setTimeSignature(1, 4, 4);
    stateManager->drainQueue();
    REQUIRE(pos() == 0);
    sequencer->goToNextStep();
    stateManager->drainQueue();
    // TODO User-friendlier would be if the next step starts at the beginning of
    // a bar, which is not the
    //  case with the above timesignatures (first bar 1/32, second bar 4/4) on
    //  the real MPC2000XL. So the below is according to spec, but maybe we can
    //  do the user-friendlier variety at some point.
    REQUIRE(pos() == 24);
    sequencer->goToNextStep();
    stateManager->drainQueue();
    REQUIRE(pos() == 48);
    sequencer->goToNextStep();
    stateManager->drainQueue();
    REQUIRE(pos() == 72);
    sequencer->goToPreviousStep();
    stateManager->drainQueue();
    REQUIRE(pos() == 48);
    sequencer->goToPreviousStep();
    stateManager->drainQueue();
    REQUIRE(pos() == 24);
    sequencer->goToPreviousStep();
    stateManager->drainQueue();
    REQUIRE(pos() == 0);
}

TEST_CASE("Next Sq takes focus even when note repeat hint is visible",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    auto seq0 = sequencer->getSequence(0);
    seq0->init(0);
    auto seq1 = sequencer->getSequence(1);
    seq1->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    const auto hwController =
        mpc.clientEventController->clientHardwareEventController;
    hwController->lockNoteRepeat();

    // Re-open so SequencerScreen::open applies the hold-hint UI branch.
    layeredScreen->openScreenById(ScreenId::StepEditorScreen);
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");

    const auto screen = mpc.getScreen();
    REQUIRE_FALSE(screen->findChild("footer-label")->IsHidden());
    REQUIRE(screen->findField("nextsq")->IsHidden());
}

TEST_CASE("Next Sq focus is preserved across note repeat press/release",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    auto seq0 = sequencer->getSequence(0);
    seq0->init(0);
    auto seq1 = sequencer->getSequence(1);
    seq1->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");
    REQUIRE_FALSE(mpc.getScreen()->findField("nextsq")->IsHidden());

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    const auto hwController =
        mpc.clientEventController->clientHardwareEventController;

    const auto tapPress = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        1.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapPress);

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");
    REQUIRE(mpc.getScreen()->findField("nextsq")->IsHidden());

    const auto tapRelease = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        0.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapRelease);

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");
    REQUIRE_FALSE(mpc.getScreen()->findField("nextsq")->IsHidden());
}

TEST_CASE("Next Sq focus is preserved when releasing TAP after locking note repeat",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    auto seq0 = sequencer->getSequence(0);
    seq0->init(0);
    auto seq1 = sequencer->getSequence(1);
    seq1->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    const auto hwController =
        mpc.clientEventController->clientHardwareEventController;

    const auto tapPress = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        1.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapPress);

    const auto shiftPress = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        SHIFT,
        1.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(shiftPress);

    REQUIRE(hwController->isNoteRepeatLocked());

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");
    REQUIRE(mpc.getScreen()->findField("nextsq")->IsHidden());

    const auto tapRelease = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        0.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapRelease);

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");
}

TEST_CASE("Next Sq focus survives lock flow when set via NEXT SEQ screen",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(1)->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    const auto hwController =
        mpc.clientEventController->clientHardwareEventController;

    const auto tapPress = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        1.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapPress);

    const auto shiftPress = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        SHIFT,
        1.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(shiftPress);
    REQUIRE(hwController->isNoteRepeatLocked());

    const auto nextSeqPress = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        NEXT_SEQ,
        1.f,
        std::nullopt,
        std::nullopt};
    const auto nextSeqRelease = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        NEXT_SEQ,
        0.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(nextSeqPress);
    hwController->handleClientHardwareEvent(nextSeqRelease);

    REQUIRE(layeredScreen->getCurrentScreenName() == "next-seq");
    mpc.getScreen()->turnWheel(1);
    stateManager->drainQueue();
    layeredScreen->timerCallback();
    REQUIRE(sequencer->getNextSq() == mpc::SequenceIndex(1));

    hwController->handleClientHardwareEvent(nextSeqPress);
    hwController->handleClientHardwareEvent(nextSeqRelease);
    REQUIRE(layeredScreen->getCurrentScreenName() == "sequencer");
    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");

    const auto tapRelease = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        0.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapRelease);

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");
}

TEST_CASE("Releasing note repeat restores Next Sq focus when next sequence exists",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(1)->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);
    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();
    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    const auto hwController =
        mpc.clientEventController->clientHardwareEventController;

    const auto tapPress = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonPress,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        1.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapPress);

    REQUIRE(mpc.getScreen()->findField("nextsq")->IsHidden());
    REQUIRE(layeredScreen->setFocus("sq"));
    REQUIRE(layeredScreen->getFocusedFieldName() == "sq");

    const auto tapRelease = ClientHardwareEvent{
        ClientHardwareEvent::Source::HostInputGesture,
        ClientHardwareEvent::Type::MpcButtonRelease,
        std::nullopt,
        TAP_TEMPO_OR_NOTE_REPEAT,
        0.f,
        std::nullopt,
        std::nullopt};
    hwController->handleClientHardwareEvent(tapRelease);

    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");
}

TEST_CASE("Focus returns to Sq after queued next sequence is consumed",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(1)->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();
    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    // Emulate playback engine behavior when sequence end is reached:
    // switch to queued sequence, then clear next sq.
    sequencer->setSelectedSequenceIndex(mpc::SequenceIndex(1), false);
    stateManager->drainQueue();
    sequencer->clearNextSq();
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::SequenceIndex(1));
    REQUIRE(sequencer->getNextSq() == mpc::NoNextSequenceIndex);
    REQUIRE(layeredScreen->getFocusedFieldName() == "sq");
}

TEST_CASE("Step editor note velocity is clamped at zero",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    mpc::sequencer::EventData eventData;
    eventData.type = mpc::sequencer::EventType::NoteOn;
    eventData.tick = 0;
    eventData.noteNumber = mpc::MinDrumNoteNumber;
    eventData.velocity = mpc::MinVelocity;
    eventData.duration = mpc::Duration(10);
    seq->getTrack(0)->acquireAndInsertEvent(eventData);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::StepEditorScreen);

    REQUIRE(layeredScreen->setFocus("e0"));
    REQUIRE(layeredScreen->getFocusedFieldName() == "e0");

    mpc.getScreen()->turnWheel(-1);
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    auto noteEvent =
        std::dynamic_pointer_cast<NoteOnEvent>(seq->getTrack(0)->getEvent(0));
    REQUIRE(noteEvent);
    REQUIRE(noteEvent->getVelocity() == mpc::MinVelocity);
}

TEST_CASE("Focus returns to Sq after stopping with queued next sequence",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(1)->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();
    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");

    mpc.getScreen()->stop();
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    REQUIRE(sequencer->getNextSq() == mpc::NoNextSequenceIndex);
    REQUIRE(layeredScreen->getFocusedFieldName() == "sq");
}

TEST_CASE("Stopping with queued next sequence preserves non-nextsq focus",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(1)->init(0);
    stateManager->drainQueue();

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::SequencerScreen);

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    layeredScreen->timerCallback();
    REQUIRE(layeredScreen->getFocusedFieldName() == "nextsq");

    REQUIRE(layeredScreen->setFocus("tempo"));
    REQUIRE(layeredScreen->getFocusedFieldName() == "tempo");

    mpc.getScreen()->stop();
    stateManager->drainQueue();
    layeredScreen->timerCallback();

    REQUIRE(sequencer->getNextSq() == mpc::NoNextSequenceIndex);
    REQUIRE(layeredScreen->getFocusedFieldName() == "tempo");
}

TEST_CASE("Can record and playback from different threads",
          "[sequencer-multithread]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    auto track = sequencer->getSelectedTrack();
    auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    timingCorrectScreen->setNoteValue(0);
    track->setUsedIfCurrentlyUnused();
    stateManager->drainQueue();

    const std::vector<mpc::Tick> processedTicks{12, 36, 60, 84, 108};
    const std::vector<mpc::NoteNumber> noteNumbers{
        mpc::NoteNumber(35), mpc::NoteNumber(36), mpc::NoteNumber(37),
        mpc::NoteNumber(38), mpc::NoteNumber(39)};

    std::mutex mutex;
    std::condition_variable cv;
    size_t publishedTickIndex = 0;
    bool tickReady = false;
    bool canProcessPublishedTick = false;

    std::thread playbackThread(
        [&]
        {
            for (size_t i = 0; i < processedTicks.size(); ++i)
            {
                std::unique_lock<std::mutex> lock(mutex);
                publishedTickIndex = i;
                tickReady = true;
                cv.notify_one();
                cv.wait(lock, [&] { return canProcessPublishedTick; });
                canProcessPublishedTick = false;
                tickReady = false;
                lock.unlock();

                const auto sequenceState =
                    stateManager->getSnapshot().getSequenceState(
                        seq->getSequenceIndex());
                stateManager->processLiveNoteEventRecordingQueues(
                    processedTicks[i], sequenceState);
            }
        });

    for (size_t i = 0; i < processedTicks.size(); ++i)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&] { return tickReady && publishedTickIndex == i; });

        auto *noteOn = stateManager->recordNoteEventLive(
            seq->getSequenceIndex(), track->getIndex(), noteNumbers[i],
            mpc::Velocity(100));
        stateManager->finalizeNoteEventLive(
            noteOn, mpc::NoTickAssignedWhileRecording);

        canProcessPublishedTick = true;
        lock.unlock();
        cv.notify_one();
    }

    playbackThread.join();
    track->setOn(track->isOn(), false);
    stateManager->drainQueue();

    const auto events = track->getEvents();
    REQUIRE(events.size() == noteNumbers.size());

    for (size_t i = 0; i < events.size(); ++i)
    {
        REQUIRE(events[i]->getTick() == processedTicks[i]);
        const auto noteOnEvent =
            std::dynamic_pointer_cast<NoteOnEvent>(events[i]);
        REQUIRE(noteOnEvent);
        REQUIRE(noteOnEvent->getNote() == noteNumbers[i]);
    }
}

TEST_CASE("Live recording quantizes loop-boundary notes deterministically",
          "[sequencer-multithread]")
{
    // Quantized positions:
    // 1                   2
    // 0  , 24 , 48 , 72 , 96 , 120, 144, 168,
    //
    // 3                   4                   1... <loop>
    // 192, 216, 240, 264, 288, 312, 336, 360, 384
    //
    // The event at tick 382 quantizes to tick 0 in a one-bar sequence. The
    // event at tick 2 also quantizes to tick 0, so only one of those two note
    // events survives after duplicate removal by note number and tick.
    const std::vector<mpc::Tick> humanTickPositions{
        2, 23, 49, 70, 95, 124, 143, 167, 194, 218, 243, 264, 290, 310, 332,
        361, 382};

    const std::vector<mpc::Tick> quantizedPositions{
        0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240, 264, 288, 312, 336,
        360};

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    auto track = sequencer->getSelectedTrack();
    auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    timingCorrectScreen->setNoteValue(3);
    track->setUsedIfCurrentlyUnused();
    stateManager->drainQueue();
    const auto sequenceState =
        stateManager->getSnapshot().getSequenceState(seq->getSequenceIndex());

    for (const auto tick : humanTickPositions)
    {
        auto *noteOn = stateManager->recordNoteEventLive(
            seq->getSequenceIndex(), track->getIndex(), mpc::NoteNumber(35),
            mpc::Velocity(100));
        stateManager->finalizeNoteEventLive(
            noteOn, mpc::NoTickAssignedWhileRecording);
        stateManager->processLiveNoteEventRecordingQueues(tick, sequenceState);
    }

    track->setOn(track->isOn(), false);
    stateManager->drainQueue();

    const auto events = track->getEvents();
    REQUIRE(events.size() == quantizedPositions.size());

    for (size_t i = 0; i < quantizedPositions.size(); ++i)
    {
        REQUIRE(events[i]->getTick() == quantizedPositions[i]);
    }
}

TEST_CASE("Copy sequence", "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto seq1 = sequencer->getSelectedSequence();
    seq1->init(2);
    stateManager->drainQueue();

    REQUIRE(seq1->getInitialTempo() == 120);

    sequencer->getTransport()->setTempo(119);

    stateManager->drainQueue();

    REQUIRE(seq1->getInitialTempo() == 119);

    sequencer->copySequence(mpc::SequenceIndex(0), mpc::SequenceIndex(1));

    auto seq2 = sequencer->getSequence(1);

    stateManager->drainQueue();

    REQUIRE(seq2->getTempoChangeEvents().size() == 1);
    REQUIRE(seq2->getInitialTempo() == 119);

    sequencer->getTransport()->setTempo(122);

    stateManager->drainQueue();

    REQUIRE(seq1->getInitialTempo() == 122);
    REQUIRE(seq2->getInitialTempo() == 119);

    sequencer->setSelectedSequenceIndex(mpc::SequenceIndex(1), false);
    stateManager->drainQueue();
    sequencer->getTransport()->setTempo(123);

    stateManager->drainQueue();

    REQUIRE(seq1->getInitialTempo() == 122);
    REQUIRE(seq2->getInitialTempo() == 123);
}

TEST_CASE("Undo", "[sequencer]")
{
    constexpr int BUFFER_SIZE = 4096;
    constexpr int SAMPLE_RATE = 44100;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    timingCorrectScreen->setNoteValue(0);

    sequencer->getSelectedSequence()->init(2);
    stateManager->drainQueue();
    sequencer->getTransport()->setTempo(121);
    stateManager->drainQueue();

    auto server = mpc.getEngineHost()->getAudioServer();
    server->resizeBuffers(BUFFER_SIZE);
    server->setSampleRate(SAMPLE_RATE);
    server->start();

    sequencer->getTransport()->recFromStart();
    mpc.getEngineHost()->prepareProcessBlock(BUFFER_SIZE);

    const auto screen = mpc.getLayeredScreen()->getCurrentScreen();

    for (int i = 0; i < 20; i++)
    {
        ClientEvent clientEvent;
        if (i % 2 == 0)
        {
            clientEvent.payload = ClientHardwareEvent{
                ClientHardwareEvent::Source::HostInputGesture,
                ClientHardwareEvent::Type::PadPress,
                0,
                PAD_1_OR_AB,
                1.f,
                std::nullopt,
                std::nullopt};
            mpc.clientEventController->handleClientEvent(clientEvent);
        }
        else
        {
            clientEvent.payload = ClientHardwareEvent{
                ClientHardwareEvent::Source::HostInputGesture,
                ClientHardwareEvent::Type::PadRelease,
                0,
                PAD_1_OR_AB,
                0.f,
                std::nullopt,
                std::nullopt};
            mpc.clientEventController->handleClientEvent(clientEvent);
            stateManager->drainQueue();
        }

        mpc.getEngineHost()->prepareProcessBlock(BUFFER_SIZE);

        mpc.getClock()->processBufferInternal(
            sequencer->getTransport()->getTempo(), SAMPLE_RATE, BUFFER_SIZE, 0);

        server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});
    }

    sequencer->getTransport()->stop();
    stateManager->drainQueue();
    REQUIRE(sequencer->getSelectedSequence()->getTrack(0)->getEvents().size() ==
            10);

    sequencer->undoSeq();

    mpc.getEngineHost()->prepareProcessBlock(BUFFER_SIZE);

    REQUIRE(sequencer->getSelectedSequence()->isUsed());
    REQUIRE(sequencer->getTransport()->getTempo() == 121);
    REQUIRE(sequencer->getSelectedSequence()->getTrack(0)->getEvents().empty());

    sequencer->undoSeq();

    mpc.getEngineHost()->prepareProcessBlock(BUFFER_SIZE);

    REQUIRE(sequencer->getSelectedSequence()->getTrack(0)->getEvents().size() ==
            10);
}

TEST_CASE("Second sequence loops independently on repeated master loops",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    seq0->init(0);
    seq1->init(0);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 0, 60);
    prepareMidiPlaybackTrack(seq1->getTrack(0), 0, 61);
    stateManager->drainQueue();

    configureSecondSequence(mpc, 1);

    sequencer->getTransport()->play(true);
    stateManager->drainQueue();

    processPlaybackBuffers(mpc, 420);

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(60) >= 2);
    REQUIRE(noteOns.at(61) >= 2);
}

TEST_CASE("Second sequence does not wrap when its own loop is disabled",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    seq0->init(0);
    seq1->init(0);
    stateManager->drainQueue();

    seq1->setTimeSignature(0, 1, 4);
    seq1->setLoopEnabled(false);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 0, 60);
    prepareMidiPlaybackTrack(seq1->getTrack(0), 0, 61);
    stateManager->drainQueue();

    configureSecondSequence(mpc, 1);

    sequencer->getTransport()->play(true);
    stateManager->drainQueue();

    processPlaybackBuffers(mpc, 420);

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(60) >= 2);
    REQUIRE(noteOns.at(61) == 1);
}

TEST_CASE("Second sequence stops with master when master loop is disabled",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    seq0->init(0);
    seq1->init(0);
    stateManager->drainQueue();

    seq0->setTimeSignature(0, 1, 4);
    seq0->setLoopEnabled(false);
    seq1->setTimeSignature(0, 1, 4);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 0, 60);
    prepareMidiPlaybackTrack(seq1->getTrack(0), 0, 61);
    stateManager->drainQueue();

    configureSecondSequence(mpc, 1);

    sequencer->getTransport()->play(true);
    stateManager->drainQueue();

    processPlaybackBuffers(mpc, 220);

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(60) == 1);
    REQUIRE(noteOns.at(61) == 1);
    REQUIRE_FALSE(sequencer->getTransport()->isPlaying());
}

TEST_CASE("Second sequence can double-play the master sequence",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    seq0->init(0);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 0, 60);
    stateManager->drainQueue();

    configureSecondSequence(mpc, 0);

    sequencer->getTransport()->play(true);
    stateManager->drainQueue();

    processPlaybackBuffers(mpc, 32);

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(60) == 2);
}

TEST_CASE("Second sequence start position follows elapsed non-zero start time",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    seq0->init(0);
    seq1->init(0);
    stateManager->drainQueue();

    seq1->setTimeSignature(0, 2, 4);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 240, 60);
    prepareMidiPlaybackTrack(seq1->getTrack(0), 48, 61);
    stateManager->drainQueue();

    configureSecondSequence(mpc, 1);

    sequencer->getTransport()->setPosition(Sequencer::ticksToQuarterNotes(240));
    stateManager->drainQueue();
    sequencer->getTransport()->play();
    stateManager->drainQueue();

    processPlaybackBuffers(mpc, 32);

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(60) == 1);
    REQUIRE(noteOns.at(61) == 1);
}

TEST_CASE("Second sequence stays continuous across song step changes",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    auto seq2 = sequencer->getSequence(2);
    seq0->init(0);
    seq1->init(0);
    seq2->init(0);
    stateManager->drainQueue();

    seq0->setTimeSignature(0, 1, 4);
    seq1->setTimeSignature(0, 1, 4);
    seq2->setTimeSignature(0, 6, 16);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 0, 60);
    prepareMidiPlaybackTrack(seq1->getTrack(0), 0, 61);
    prepareMidiPlaybackTrack(seq2->getTrack(0), 96, 62);
    stateManager->drainQueue();

    auto song = sequencer->getSong(0);
    song->setUsed(true);
    song->setStepSequenceIndex(mpc::SongStepIndex(0), mpc::SequenceIndex(0));
    song->setStepSequenceIndex(mpc::SongStepIndex(1), mpc::SequenceIndex(1));
    stateManager->drainQueue();

    configureSecondSequence(mpc, 2);
    mpc.getLayeredScreen()->openScreenById(ScreenId::SongScreen);

    sequencer->getTransport()->play(true);
    stateManager->drainQueue();

    processPlaybackBuffers(mpc, 120);

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(60) == 1);
    REQUIRE(noteOns.at(61) == 1);
    REQUIRE(noteOns.at(62) == 1);
}

TEST_CASE("Second sequence stays read-only during punched recording",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    seq0->init(0);
    seq1->init(0);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 0, 60);
    prepareMidiPlaybackTrack(seq1->getTrack(0), 0, 61);
    stateManager->drainQueue();

    configureSecondSequence(mpc, 1);

    auto transport = sequencer->getTransport();
    transport->setPunchEnabled(true);
    transport->setAutoPunchMode(2);
    transport->setPunchInTime(0);
    transport->setPunchOutTime(96);

    transport->recFromStart();
    stateManager->drainQueue();

    processPlaybackBuffers(mpc, 32);
    transport->stop();
    stateManager->drainQueue();

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(61) == 1);
    REQUIRE(seq0->getTrack(0)->getEvents().empty());
    REQUIRE(seq1->getTrack(0)->getEvents().size() == 1);
    REQUIRE(seq1->getTrack(0)->getEvent(0)->getTick() == 0);
}

TEST_CASE("Second sequence repositions from absolute elapsed time on host jump",
          "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto syncScreen = mpc.screens->get<ScreenId::SyncScreen>();
    sequencer->getTransport()->setCountEnabled(false);
    syncScreen->modeIn = 1;

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    seq0->init(0);
    seq1->init(0);
    stateManager->drainQueue();

    seq1->setTimeSignature(0, 2, 4);
    stateManager->drainQueue();

    prepareMidiPlaybackTrack(seq0->getTrack(0), 241, 60);
    prepareMidiPlaybackTrack(seq1->getTrack(0), 49, 61);
    stateManager->drainQueue();

    configureSecondSequence(mpc, 1);

    sequencer->getTransport()->play();
    stateManager->drainQueue();

    processExternalPlaybackBuffer(mpc, 0.0, kPlaybackBufferSize, 0);
    processExternalPlaybackBuffer(mpc, 0.1, kPlaybackBufferSize,
                                  kPlaybackBufferSize);
    processExternalPlaybackBuffer(
        mpc, Sequencer::ticksToQuarterNotes(240), kPlaybackBufferSize,
        kPlaybackBufferSize * 4);
    processExternalPlaybackBuffer(
        mpc, Sequencer::ticksToQuarterNotes(245), kPlaybackBufferSize,
        kPlaybackBufferSize * 5);

    const auto noteOns = dequeueMidiNoteOnCounts(mpc);
    REQUIRE(noteOns.at(60) == 1);
    REQUIRE(noteOns.at(61) == 1);
}

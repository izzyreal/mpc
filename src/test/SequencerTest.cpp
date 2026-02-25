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
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "performance/PerformanceManager.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

#include "hardware/Component.hpp"
#include "sequencer/EventRef.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::client::event;
using namespace mpc::hardware;
using namespace mpc::lcdgui;

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

TEST_CASE("Transport tick position advances monotonically during playback",
          "[sequencer]")
{
    constexpr int BUFFER_SIZE = 512;
    constexpr int SAMPLE_RATE = 44100;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto transport = sequencer->getTransport();

    transport->setCountEnabled(false);
    sequencer->getSelectedSequence()->init(4);
    stateManager->drainQueue();

    auto server = mpc.getEngineHost()->getAudioServer();
    server->resizeBuffers(BUFFER_SIZE);
    server->setSampleRate(SAMPLE_RATE);
    server->start();

    transport->play();
    stateManager->drainQueue();

    int previousTick = transport->getTickPosition();

    for (int i = 0; i < 40; i++)
    {
        mpc.getEngineHost()->prepareProcessBlock(BUFFER_SIZE);
        mpc.getClock()->processBufferInternal(
            transport->getTempo(), SAMPLE_RATE, BUFFER_SIZE, 0);
        server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});
        stateManager->drainQueue();

        const auto currentTick = transport->getTickPosition();
        REQUIRE(currentTick >= previousTick);
        previousTick = currentTick;
    }

    REQUIRE(transport->getTickPosition() > 0);
}

TEST_CASE("Queued Next Sq is consumed at boundary during playback",
          "[sequencer]")
{
    constexpr int BUFFER_SIZE = 2048;
    constexpr int SAMPLE_RATE = 44100;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto transport = sequencer->getTransport();

    transport->setCountEnabled(false);

    auto seq0 = sequencer->getSequence(0);
    auto seq1 = sequencer->getSequence(1);
    seq0->init(0);
    seq1->init(0);
    stateManager->drainQueue();

    sequencer->setSelectedSequenceIndex(mpc::SequenceIndex(0), false);
    stateManager->drainQueue();

    const auto nearBoundary =
        Sequencer::ticksToQuarterNotes(seq0->getLastTick() - 1);
    transport->setPosition(nearBoundary);
    stateManager->drainQueue();

    sequencer->setNextSq(mpc::SequenceIndex(1), true);
    stateManager->drainQueue();
    REQUIRE(sequencer->getNextSq() == mpc::SequenceIndex(1));

    auto server = mpc.getEngineHost()->getAudioServer();
    server->resizeBuffers(BUFFER_SIZE);
    server->setSampleRate(SAMPLE_RATE);
    server->start();

    transport->play();
    stateManager->drainQueue();

    mpc.getEngineHost()->prepareProcessBlock(BUFFER_SIZE);
    mpc.getClock()->processBufferInternal(transport->getTempo(), SAMPLE_RATE,
                                          BUFFER_SIZE, 0);
    server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});
    stateManager->drainQueue();

    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::SequenceIndex(1));
    REQUIRE(sequencer->getNextSq() == mpc::NoNextSequenceIndex);
}

TEST_CASE("Can record and playback from different threads",
          "[sequencer-multithread]")
{
    constexpr int SAMPLE_RATE = 44100;
    constexpr int BUFFER_SIZE = 512;
    constexpr int AUDIO_THREAD_MAX_CYCLES = 4000;

    // Quantized positions:
    // 1                   2
    // 0  , 24 , 48 , 72 , 96 , 120, 144, 168,
    //
    // 3                   4                   1... <loop>
    // 192, 216, 240, 264, 288, 312, 336, 360, 384
    //
    // The event at tick 382 is expected to be quantized to tick 0, because the
    // sequence is one bar long. Event at tick 2 will also be quantized to tick
    // 0. Since the events have the same note number, and during recording no
    // duplicate note numbers per tick position are left over, of the 17 ticks
    // below, only 16 will survive.
    std::vector humanTickPositions{2,   23,  49,  70,  95,  124, 143, 167, 194,
                                   218, 243, 264, 290, 310, 332, 361, 382};

    std::vector quantizedPositions{0,   24,  48,  72,  96,  120, 144, 168,
                                   192, 216, 240, 264, 288, 312, 336, 360};

    std::atomic<bool> audioThreadBusy{true};
    std::atomic<bool> stopAudioThread{false};
    std::mutex tickMutex;
    std::condition_variable tickCv;
    int publishedTickPos = 0;
    bool tickUpdated = false;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    auto seq = sequencer->getSelectedSequence();
    seq->init(0);
    stateManager->drainQueue();

    auto track = sequencer->getSelectedTrack();

    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(BUFFER_SIZE);
    server->setSampleRate(SAMPLE_RATE);
    server->start();

    std::thread audioThread(
        [&]
        {
            int dspCycleCounter = 0;
            while (dspCycleCounter++ < AUDIO_THREAD_MAX_CYCLES &&
                   !stopAudioThread.load(std::memory_order_acquire))
            {
                mpc.getEngineHost()->prepareProcessBlock(BUFFER_SIZE);
                mpc.getClock()->processBufferInternal(
                    sequencer->getTransport()->getTempo(), SAMPLE_RATE,
                    BUFFER_SIZE, 0);
                server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});

                std::unique_lock<std::mutex> lock(tickMutex);
                publishedTickPos = sequencer->getTransport()->getTickPosition();
                tickUpdated = true;
                tickCv.notify_one();
                tickCv.wait(lock, [&]
                            {
                                return !tickUpdated ||
                                       stopAudioThread.load(
                                           std::memory_order_acquire);
                            });
            }

            audioThreadBusy.store(false, std::memory_order_release);
            tickCv.notify_one();
        });

    if (!sequencer->getTransport()->isRecordingOrOverdubbing())
    {
        sequencer->getTransport()->recFromStart();
    }

    size_t nextHumanTickPosIndex = 0;
    int prevTickPos = -1;
    bool wrapped = false;

    auto triggerPad = [&mpc]
    {
        ClientEvent clientEvent;
        clientEvent.payload = ClientHardwareEvent{
            ClientHardwareEvent::Source::HostInputGesture,
            ClientHardwareEvent::Type::PadPress,
            0,
            PAD_1_OR_AB,
            1.f,
            std::nullopt,
            std::nullopt};
        mpc.clientEventController->handleClientEvent(clientEvent);

        std::this_thread::sleep_for(std::chrono::milliseconds(2));

        clientEvent.payload = ClientHardwareEvent{
            ClientHardwareEvent::Source::HostInputGesture,
            ClientHardwareEvent::Type::PadRelease,
            0,
            PAD_1_OR_AB,
            1.f,
            std::nullopt,
            std::nullopt};
        mpc.clientEventController->handleClientEvent(clientEvent);
    };

    while (nextHumanTickPosIndex < humanTickPositions.size())
    {
        std::unique_lock<std::mutex> lock(tickMutex);
        tickCv.wait(lock, [&]
                    {
                        return tickUpdated ||
                               !audioThreadBusy.load(std::memory_order_acquire);
                    });

        if (!audioThreadBusy.load(std::memory_order_acquire) && !tickUpdated)
        {
            break;
        }

        const auto tickPos = publishedTickPos;
        tickUpdated = false;
        lock.unlock();
        tickCv.notify_one();

        if (prevTickPos >= 0 && tickPos < prevTickPos)
        {
            wrapped = true;
            break;
        }

        while (nextHumanTickPosIndex < humanTickPositions.size() &&
               tickPos >= humanTickPositions[nextHumanTickPosIndex])
        {
            triggerPad();
            nextHumanTickPosIndex++;
        }

        prevTickPos = tickPos;
    }

    sequencer->getTransport()->stop();

    stopAudioThread.store(true, std::memory_order_release);
    tickCv.notify_one();

    while (audioThreadBusy.load(std::memory_order_acquire))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    audioThread.join();

    sequencer->getStateManager()->drainQueue();

    REQUIRE(!wrapped);
    REQUIRE(nextHumanTickPosIndex == humanTickPositions.size());

    // For - 1 explanation, see humanTickPositions comment
    REQUIRE(track->getEvents().size() == humanTickPositions.size() - 1);
    REQUIRE(track->getEvents().size() == quantizedPositions.size());

    for (int i = 0; i < quantizedPositions.size(); ++i)
    {
        REQUIRE(track->getEvent(i)->getTick() == quantizedPositions[i]);
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

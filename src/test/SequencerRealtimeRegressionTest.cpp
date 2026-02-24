#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "engine/EngineHost.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "lcdgui/ScreenId.hpp"
#include "sequencer/Clock.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/SequencerMessage.hpp"

#include <algorithm>

namespace
{
    constexpr int kSampleRate = 44100;
    constexpr int kBufferSize = 2048;
    constexpr int kStressSampleRate = 100;
    constexpr int kStressBufferSize = 256;

    void runRealtimeCycle(mpc::Mpc &mpc)
    {
        auto engineHost = mpc.getEngineHost();
        auto sequencer = mpc.getSequencer();
        auto server = engineHost->getAudioServer();

        engineHost->prepareProcessBlock(kBufferSize);
        mpc.getClock()->processBufferInternal(
            sequencer->getTransport()->getTempo(), kSampleRate, kBufferSize, 0);
        server->work(nullptr, nullptr, kBufferSize, {}, {}, {}, {});
    }
} // namespace

TEST_CASE("Transport advances by all ticks in a DSP cycle", "[sequencer-realtime]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(kBufferSize);
    server->setSampleRate(kSampleRate);
    server->start();

    sequencer->getSelectedSequence()->init(8);
    sequencer->getTransport()->setCountEnabled(false);
    stateManager->drainQueue();

    sequencer->getTransport()->play(true);

    mpc.getEngineHost()->prepareProcessBlock(kBufferSize);
    mpc.getClock()->processBufferInternal(
        sequencer->getTransport()->getTempo(), kSampleRate, kBufferSize, 0);

    const auto expectedTicksThisCycle =
        static_cast<int>(mpc.getClock()->getTicksForCurrentBuffer().size());

    server->work(nullptr, nullptr, kBufferSize, {}, {}, {}, {});

    REQUIRE(expectedTicksThisCycle > 1);
    REQUIRE(sequencer->getTransport()->getTickPosition() ==
            expectedTicksThisCycle);
}

TEST_CASE("Sudden sequence switch remains correct during realtime playback",
          "[sequencer-realtime]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(kBufferSize);
    server->setSampleRate(kSampleRate);
    server->start();

    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(1)->init(0);
    sequencer->getTransport()->setCountEnabled(false);
    stateManager->drainQueue();

    sequencer->getTransport()->play(true);
    runRealtimeCycle(mpc);

    stateManager->enqueue(mpc::sequencer::SwitchToNextSequenceSudden{
        mpc::SequenceIndex(1)});
    runRealtimeCycle(mpc);

    REQUIRE(sequencer->getCurrentlyPlayingSequenceIndex() == mpc::SequenceIndex(1));
    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::SequenceIndex(1));
    REQUIRE(sequencer->getTransport()->getTickPosition() <
            sequencer->getSequence(1)->getLastTick());
}

TEST_CASE("Count-in exits correctly while running realtime cycles",
          "[sequencer-realtime]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(kBufferSize);
    server->setSampleRate(kSampleRate);
    server->start();

    sequencer->getSelectedSequence()->init(0);
    sequencer->getTransport()->setCountEnabled(true);
    stateManager->drainQueue();

    sequencer->getTransport()->recFromStart();

    bool countingInStarted = false;
    bool countingInStopped = false;

    for (int i = 0; i < 60; i++)
    {
        runRealtimeCycle(mpc);

        if (sequencer->getTransport()->isCountingIn())
        {
            countingInStarted = true;
        }
        else if (countingInStarted)
        {
            countingInStopped = true;
            break;
        }
    }

    REQUIRE(countingInStarted);
    REQUIRE(countingInStopped);
}

TEST_CASE("Song step and repetition updates remain correct in realtime",
          "[sequencer-realtime]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(kBufferSize);
    server->setSampleRate(kSampleRate);
    server->start();

    sequencer->getSequence(0)->init(0);
    sequencer->getSequence(1)->init(0);

    const auto song = sequencer->getSelectedSong();
    song->setUsed(true);
    song->insertStep(mpc::SongStepIndex(0));
    song->setStepSequenceIndex(mpc::SongStepIndex(0), mpc::SequenceIndex(0));
    song->setStepRepetitionCount(mpc::SongStepIndex(0), 2);
    song->insertStep(mpc::SongStepIndex(1));
    song->setStepSequenceIndex(mpc::SongStepIndex(1), mpc::SequenceIndex(1));
    song->setStepRepetitionCount(mpc::SongStepIndex(1), 1);

    sequencer->setSelectedSongStepIndex(mpc::SongStepIndex(0));
    sequencer->getTransport()->setCountEnabled(false);
    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(mpc::lcdgui::ScreenId::SongScreen);
    REQUIRE(sequencer->isSongModeEnabled());

    sequencer->getTransport()->play(true);
    runRealtimeCycle(mpc);

    sequencer->setSelectedSongStepIndex(mpc::SongStepIndex(1));
    sequencer->getTransport()->incrementPlayedStepRepetitions();
    runRealtimeCycle(mpc);

    REQUIRE(sequencer->getSelectedSongStepIndex() == mpc::SongStepIndex(1));
    REQUIRE(sequencer->getCurrentlyPlayingSequenceIndex() == mpc::SequenceIndex(1));
    REQUIRE(sequencer->getTransport()->getPlayedStepRepetitions() == 1);
}

TEST_CASE("Stress cycle advances transport by all generated ticks",
          "[sequencer-realtime]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(kStressBufferSize);
    server->setSampleRate(kStressSampleRate);
    server->start();

    sequencer->getSelectedSequence()->init(8);
    sequencer->getTransport()->setCountEnabled(false);
    sequencer->getTransport()->setTempoSourceIsSequence(false);
    sequencer->getTransport()->setMasterTempo(300.0);
    stateManager->drainQueue();

    sequencer->getTransport()->play(true);

    mpc.getEngineHost()->prepareProcessBlock(kStressBufferSize);
    mpc.getClock()->processBufferInternal(
        sequencer->getTransport()->getTempo(), kStressSampleRate,
        kStressBufferSize, 0);

    const auto expectedTicksThisCycle = static_cast<int>(
        mpc.getClock()->getTicksForCurrentBuffer().size());
    REQUIRE(expectedTicksThisCycle > 0);

    server->work(nullptr, nullptr, kStressBufferSize, {}, {}, {}, {});

    REQUIRE(sequencer->getTransport()->getTickPosition() ==
            expectedTicksThisCycle);
}

TEST_CASE("Metronome-only position advances by every generated tick",
          "[sequencer-realtime]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(kStressBufferSize);
    server->setSampleRate(kStressSampleRate);
    server->start();

    sequencer->getTransport()->setTempoSourceIsSequence(false);
    sequencer->getTransport()->setMasterTempo(300.0);
    stateManager->drainQueue();

    sequencer->getTransport()->playMetronomeOnly();

    const auto posBefore =
        stateManager->getSnapshot().getTransportStateView()
            .getMetronomeOnlyPositionTicks();

    mpc.getEngineHost()->prepareProcessBlock(kStressBufferSize);
    server->work(nullptr, nullptr, kStressBufferSize, {}, {}, {}, {});

    const auto posAfterFirstCycle =
        stateManager->getSnapshot().getTransportStateView()
            .getMetronomeOnlyPositionTicks();

    mpc.getEngineHost()->prepareProcessBlock(kStressBufferSize);
    server->work(nullptr, nullptr, kStressBufferSize, {}, {}, {}, {});

    const auto transportSnapshot =
        stateManager->getSnapshot().getTransportStateView();

    REQUIRE(transportSnapshot.isMetronomeOnlyEnabled());
    REQUIRE(posAfterFirstCycle > posBefore);
    REQUIRE(transportSnapshot.getMetronomeOnlyPositionTicks() >
            posAfterFirstCycle);
}

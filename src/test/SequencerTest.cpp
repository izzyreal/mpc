#include <catch2/catch_test_macros.hpp>
#include "sequencer/Transport.hpp"

#include "TestMpc.hpp"
#include "client/event/ClientEvent.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "sequencer/Clock.hpp"

#include "command/TriggerLocalNoteOffCommand.hpp"
#include "command/context/TriggerLocalNoteContextFactory.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"

#include "engine/EngineHost.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "eventregistry/EventRegistry.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include "hardware/Component.hpp"

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
    auto pos = [&]
    {
        return mpc.getSequencer()->getTransport()->getTickPosition();
    };
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(1);
    seq->setTimeSignature(0, 1, 32);
    seq->setTimeSignature(1, 4, 4);
    REQUIRE(pos() == 0);
    mpc.getSequencer()->goToNextStep();
    mpc.getSequencer()->getStateManager()->drainQueue();
    // TODO User-friendlier would be if the next step starts at the beginning of
    // a bar, which is not the
    //  case with the above timesignatures (first bar 1/32, second bar 4/4) on
    //  the real MPC2000XL. So the below is according to spec, but maybe we can
    //  do the user-friendlier variety at some point.
    REQUIRE(pos() == 24);
    mpc.getSequencer()->goToNextStep();
    mpc.getSequencer()->getStateManager()->drainQueue();
    REQUIRE(pos() == 48);
    mpc.getSequencer()->goToNextStep();
    mpc.getSequencer()->getStateManager()->drainQueue();
    REQUIRE(pos() == 72);
    mpc.getSequencer()->goToPreviousStep();
    mpc.getSequencer()->getStateManager()->drainQueue();
    REQUIRE(pos() == 48);
    mpc.getSequencer()->goToPreviousStep();
    mpc.getSequencer()->getStateManager()->drainQueue();
    REQUIRE(pos() == 24);
    mpc.getSequencer()->goToPreviousStep();
    mpc.getSequencer()->getStateManager()->drainQueue();
    REQUIRE(pos() == 0);
}

TEST_CASE("Can record and playback from different threads", "[sequencer-multithread]")
{
    const int SAMPLE_RATE = 44100;
    const int BUFFER_SIZE = 512;
    const int PROCESS_BLOCK_INTERVAL =
        12; // Approximate duration of 512 frames at 44100khz
    const int AUDIO_THREAD_TIMEOUT = 4000;
    const int RECORD_DELAY = 500;
    const int INITIAL_EVENT_INSERTION_DELAY = 500;

    //                      1                   2                   3 4 1...
    //                      <loop>
    // Quantized positions: 0  , 24 , 48 , 72 , 96 , 120, 144, 168, 192, 216,
    // 240, 264, 288, 312, 336, 360, 384

    // The event at tick 382 is expected to be quantized to tick 0, because the
    // sequence is one bar long. Event at tick 2 will also be quantized to tick
    // 0. Hence of the 17 ticks below, only 16 will survive.
    std::vector humanTickPositions{2,   23,  49,  70,  95,  124, 143, 167, 194,
                                   218, 243, 264, 290, 310, 332, 361, 382};

    std::vector quantizedPositions{0,   24,  48,  72,  96,  120, 144, 168,
                                   192, 216, 240, 264, 288, 312, 336, 360};

    bool mainThreadBusy = true;
    bool audioThreadBusy = true;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto seq = mpc.getSequencer();
    seq->getTransport()->setCountEnabled(false);

    auto sequence = seq->getActiveSequence();
    sequence->init(0);

    auto track = seq->getActiveTrack();

    auto server = mpc.getEngineHost()->getAudioServer();

    server->resizeBuffers(BUFFER_SIZE);
    server->setSampleRate(SAMPLE_RATE);
    server->start();

    int64_t timeInSamples = 0;

    std::thread audioThread(
        [&]
        {
            int dspCycleCounter = 0;

            while (dspCycleCounter++ * PROCESS_BLOCK_INTERVAL <
                       AUDIO_THREAD_TIMEOUT &&
                   track->getEvents().size() < humanTickPositions.size())
            {
                mpc.getClock()->processBufferInternal(
                    seq->getTransport()->getTempo(), SAMPLE_RATE, BUFFER_SIZE,
                    0);
                server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});
                timeInSamples += BUFFER_SIZE;

                if (dspCycleCounter * PROCESS_BLOCK_INTERVAL < RECORD_DELAY)
                {
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(PROCESS_BLOCK_INTERVAL));
                    continue;
                }

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(PROCESS_BLOCK_INTERVAL));
            }

            audioThreadBusy = false;
        });

    int initialDelayCounter = 0;

    while (initialDelayCounter++ * 10 < INITIAL_EVENT_INSERTION_DELAY)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    int tickPos = seq->getTransport()->getTickPosition();

    if (!seq->getTransport()->isRecordingOrOverdubbing())
    {
        seq->getTransport()->recFromStart();
    }

    std::vector<int> recordedTickPos;
    int prevTickPos = -1;

    const auto screen = mpc.getLayeredScreen()->getCurrentScreen();

    while (tickPos < 384 && prevTickPos <= tickPos)
    {
        for (int i = 0; i < humanTickPositions.size(); i++)
        {
            auto hTickPos = humanTickPositions[i];

            if (tickPos >= hTickPos && tickPos < hTickPos + 24)
            {
                if (find(begin(recordedTickPos), end(recordedTickPos),
                         hTickPos) == end(recordedTickPos))
                {
                    ClientEvent clientEvent;
                    clientEvent.payload = ClientHardwareEvent{
                        ClientHardwareEvent::Source::HostInputGesture,
                        ClientHardwareEvent::Type::PadPress,
                        0,
                        PAD_1_OR_AB,
                        127.f,
                        std::nullopt,
                        std::nullopt};
                    mpc.clientEventController->handleClientEvent(clientEvent);

                    std::this_thread::sleep_for(std::chrono::milliseconds(2));

                    clientEvent.payload = ClientHardwareEvent{
                        ClientHardwareEvent::Source::HostInputGesture,
                        ClientHardwareEvent::Type::PadRelease,
                        0,
                        PAD_1_OR_AB,
                        127.f,
                        std::nullopt,
                        std::nullopt};
                    mpc.clientEventController->handleClientEvent(clientEvent);
                    recordedTickPos.push_back(hTickPos);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        prevTickPos = tickPos;
        tickPos = seq->getTransport()->getTickPosition();
    }

    seq->getTransport()->stop();

    while (audioThreadBusy)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    audioThread.join();

    // For - 1 explanation, see humanTickPositions comment
    REQUIRE(track->getEvents().size() == humanTickPositions.size() - 1);
}

TEST_CASE("Copy sequence", "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    sequencer->getTransport()->setTempo(121);

    REQUIRE(sequencer->getTransport()->getTempo() == 121);

    auto seq1 = sequencer->getActiveSequence();
    seq1->init(2);

    REQUIRE(seq1->getInitialTempo() == 120);

    sequencer->getTransport()->setTempo(119);

    REQUIRE(seq1->getInitialTempo() == 119);

    sequencer->copySequence(0, 1);

    auto seq2 = sequencer->getSequence(1);

    REQUIRE(seq2->getTempoChangeEvents().size() == 1);
    REQUIRE(seq2->getInitialTempo() == 119);

    sequencer->getTransport()->setTempo(122);

    REQUIRE(seq1->getInitialTempo() == 122);
    REQUIRE(seq2->getInitialTempo() == 119);

    sequencer->setActiveSequenceIndex(1, false);
    sequencer->getTransport()->setTempo(123);

    REQUIRE(seq1->getInitialTempo() == 122);
    REQUIRE(seq2->getInitialTempo() == 123);
}

TEST_CASE("Undo", "[sequencer]")
{
    const int BUFFER_SIZE = 4096;
    const int SAMPLE_RATE = 44100;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto sequencer = mpc.getSequencer();
    sequencer->getTransport()->setCountEnabled(false);

    auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    timingCorrectScreen->setNoteValue(0);

    auto seq = sequencer->getActiveSequence();
    seq->init(2);

    sequencer->getTransport()->setTempo(121);

    auto server = mpc.getEngineHost()->getAudioServer();
    server->resizeBuffers(BUFFER_SIZE);
    server->setSampleRate(SAMPLE_RATE);
    server->start();

    sequencer->getTransport()->recFromStart();

    int64_t timeInSamples = 0;

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
                127.f,
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
                127.f,
                std::nullopt,
                std::nullopt};
            mpc.clientEventController->handleClientEvent(clientEvent);
        }

        mpc.getClock()->processBufferInternal(
            sequencer->getTransport()->getTempo(), SAMPLE_RATE, BUFFER_SIZE, 0);

        server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});
        timeInSamples += BUFFER_SIZE;
    }

    sequencer->getTransport()->stop();
    auto tr = seq->getTrack(0);
    REQUIRE(tr->getEvents().size() == 10);

    sequencer->undoSeq();

    seq = sequencer->getActiveSequence();
    tr = seq->getTrack(0);

    REQUIRE(seq->isUsed());
    REQUIRE(sequencer->getTransport()->getTempo() == 121);
    REQUIRE(tr->getEvents().empty());

    sequencer->undoSeq();

    seq = sequencer->getActiveSequence();
    tr = seq->getTrack(0);
    REQUIRE(tr->getEvents().size() == 10);
}

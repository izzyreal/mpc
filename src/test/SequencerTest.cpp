#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Clock.hpp"

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <controls/GlobalReleaseControls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <engine/audio/server/NonRealTimeAudioServer.hpp>

#include <sequencer/Track.hpp>

#include <file/all/Defaults.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;
using namespace std;

TEST_CASE("Next step, previous step", "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto pos = [&]{ return mpc.getSequencer()->getTickPosition();};
    auto seq = mpc.getSequencer()->getSequence(0);
    seq->init(1);
    seq->setTimeSignature(0, 1, 32);
    seq->setTimeSignature(1, 4, 4);
    REQUIRE(pos() == 0);
    mpc.getSequencer()->goToNextStep();
    // TODO User-friendlier would be if the next step starts at the beginning of a bar, which is not the
    //  case with the above timesignatures (first bar 1/32, second bar 4/4) on the real MPC2000XL.
    //  So the below is according to spec, but maybe we can do the user-friendlier variety at some point.
    REQUIRE(pos() == 24);
    mpc.getSequencer()->goToNextStep();
    REQUIRE(pos() == 48);
    mpc.getSequencer()->goToNextStep();
    REQUIRE(pos() == 72);
    mpc.getSequencer()->goToPreviousStep();
    REQUIRE(pos() == 48);
    mpc.getSequencer()->goToPreviousStep();
    REQUIRE(pos() == 24);
    mpc.getSequencer()->goToPreviousStep();
    REQUIRE(pos() == 0);
}

TEST_CASE("Can record and playback from different threads", "[sequencer]")
{
    const int SAMPLE_RATE = 44100;
    const int BUFFER_SIZE = 512;
    const int PROCESS_BLOCK_INTERVAL = 12; // Approximate duration of 512 frames at 44100khz
    const int AUDIO_THREAD_TIMEOUT = 4000;
    const int RECORD_DELAY = 500;
    const int INITIAL_EVENT_INSERTION_DELAY = 500;

    //                      1                   2                   3                   4                   1... <loop>
    // Quantized positions: 0  , 24 , 48 , 72 , 96 , 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 360, 384

    // The event at tick 382 is expected to be quantized to tick 0, because the sequence is one bar long.
    // Event at tick 2 will also be quantized to tick 0. Hence of the 17 ticks below, only 16 will survive.
    vector<int> humanTickPositions{2, 23, 49, 70, 95, 124, 143, 167, 194, 218, 243, 264, 290, 310, 332, 361, 382};

    vector<int> quantizedPositions{0, 24, 48, 72, 96, 120, 144, 168, 192, 216, 240, 264, 288, 312, 336, 360};

    bool mainThreadBusy = true;
    bool audioThreadBusy = true;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto seq = mpc.getSequencer();
    seq->setCountEnabled(false);

    auto sequence = seq->getActiveSequence();
    sequence->init(0);

    auto track = seq->getActiveTrack();

    auto server = mpc.getAudioMidiServices()->getAudioServer();

    server->resizeBuffers(BUFFER_SIZE);

    server->setSampleRate(SAMPLE_RATE);

    int64_t timeInSamples = 0;

    thread audioThread([&]() {

        int dspCycleCounter = 0;

        while (dspCycleCounter++ * PROCESS_BLOCK_INTERVAL < AUDIO_THREAD_TIMEOUT &&
               track->getEvents().size() < humanTickPositions.size())
        {
            mpc.getClock()->processBufferInternal(seq->getTempo(), SAMPLE_RATE, BUFFER_SIZE, 0);
            server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});
            timeInSamples += BUFFER_SIZE;

            if (dspCycleCounter * PROCESS_BLOCK_INTERVAL < RECORD_DELAY)
            {
                this_thread::sleep_for(chrono::milliseconds(PROCESS_BLOCK_INTERVAL));
                continue;
            }

            this_thread::sleep_for(chrono::milliseconds(PROCESS_BLOCK_INTERVAL));
        }

        audioThreadBusy = false;
    });

    int initialDelayCounter = 0;

    while (initialDelayCounter++ * 10 < INITIAL_EVENT_INSERTION_DELAY)
        this_thread::sleep_for(chrono::milliseconds(10));

    int tickPos = seq->getTickPosition();

    if (!seq->isRecordingOrOverdubbing())
        seq->recFromStart();

    vector<int> recordedTickPos;
    int prevTickPos = -1;

    while (tickPos < 384 && prevTickPos <= tickPos)
    {
        for (int i = 0; i < humanTickPositions.size(); i++)
        {
            auto hTickPos = humanTickPositions[i];

            if (tickPos >= hTickPos && tickPos < hTickPos + 24)
            {
                if (find(begin(recordedTickPos), end(recordedTickPos), hTickPos) == end(recordedTickPos))
                {
                    mpc.getActiveControls()->pad(0, 127);

                    this_thread::sleep_for(chrono::milliseconds(2));

                    mpc.getReleaseControls()->simplePad(0);
                }
            }
        }

        this_thread::sleep_for(chrono::milliseconds(5));
        prevTickPos = tickPos;
        tickPos = seq->getTickPosition();
    }

    seq->stop();

    while (audioThreadBusy)
        this_thread::sleep_for(chrono::milliseconds(10));

    audioThread.join();

    // For - 1 explanation, see humanTickPositions comment
    REQUIRE(track->getEvents().size() == humanTickPositions.size() - 1);
}

TEST_CASE("Copy sequence", "[sequencer]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    sequencer->setTempo(121);

    REQUIRE(sequencer->getTempo() == 121);

    auto seq1 = sequencer->getActiveSequence();
    seq1->init(2);

    REQUIRE(seq1->getInitialTempo() == 120);

    sequencer->setTempo(119);

    REQUIRE(seq1->getInitialTempo() == 119);

    sequencer->copySequence(0, 1);

    auto seq2 = sequencer->getSequence(1);

    REQUIRE(seq2->getTempoChangeEvents().size() == 1);
    REQUIRE(seq2->getInitialTempo() == 119);

    sequencer->setTempo(122);

    REQUIRE(seq1->getInitialTempo() == 122);
    REQUIRE(seq2->getInitialTempo() == 119);

    sequencer->setActiveSequenceIndex(1);
    sequencer->setTempo(123);

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
    sequencer->setCountEnabled(false);

    auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
    timingCorrectScreen->setNoteValue(0);

    auto seq = sequencer->getActiveSequence();
    seq->init(2);

    sequencer->setTempo(121);

    auto server = mpc.getAudioMidiServices()->getAudioServer();
    server->resizeBuffers(BUFFER_SIZE);
    server->setSampleRate(SAMPLE_RATE);

    sequencer->recFromStart();

    auto pads = mpc.getHardware()->getPads();

    int64_t timeInSamples = 0;

    for (int i = 0; i < 20; i++)
    {
        if (i % 2 == 0) pads[0]->push(127); else pads[0]->release();

        mpc.getClock()->processBufferInternal(sequencer->getTempo(), SAMPLE_RATE, BUFFER_SIZE, 0);
        server->work(nullptr, nullptr, BUFFER_SIZE, {}, {}, {}, {});
        timeInSamples += BUFFER_SIZE;
    }

    sequencer->stop();
    auto tr = seq->getTrack(0);
    REQUIRE(tr->getEvents().size() == 10);

    sequencer->undoSeq();

    seq = sequencer->getActiveSequence();
    tr = seq->getTrack(0);

    REQUIRE(seq->isUsed());
    REQUIRE(sequencer->getTempo() == 121);
    REQUIRE(tr->getEvents().empty());

    sequencer->undoSeq();

    seq = sequencer->getActiveSequence();
    tr = seq->getTrack(0);
    REQUIRE(tr->getEvents().size() == 10);
}

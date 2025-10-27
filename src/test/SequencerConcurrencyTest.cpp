#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "sequencer/Clock.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "sequencer/Track.hpp"

#include "file/all/Defaults.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/Component.hpp"

#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;

TEST_CASE("Reproduce race: setCurrentlyPlayingSequenceIndex() vs notify*()", "[race]")
{
    // Parameters you can tweak to increase chance of reproducing a race
    const int DURATION_MS = 4000;        // total run time
    const int AUDIO_THREAD_SLEEP_US = 100;   // how long the audio thread sleeps between mutations
    const int NOTIFY_THREAD_SLEEP_US = 50;   // how long the notify-loop sleeps between calls

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    auto seq = mpc.getSequencer();
    REQUIRE(seq != nullptr);

    // prepare sequencer state so the index values we set are valid
    // setNextSq to 0 for deterministic behaviour
    seq->setNextSq(0);
    seq->setCurrentlyPlayingSequenceIndex(0);

    std::atomic<bool> stop{false};
    std::atomic<bool> audioThreadCrashed{false};
    std::atomic<bool> notifyThreadCrashed{false};

    // AUDIO-like thread: repeatedly tries to swap the currently playing sequence index
    std::thread audioThread([&]() {
        try {
            auto start = std::chrono::steady_clock::now();
            while (!stop.load()) {
                // This is the mutating call that previously caused crashes when
                // run concurrently with notify* calls.
                // We intentionally call setCurrentlyPlayingSequenceIndex(getNextSq())
                // because it's the exact sequence of operations you reported.
                seq->setCurrentlyPlayingSequenceIndex(seq->getNextSq());

                // small sleep to let notify thread run and to increase interleavings
                std::this_thread::sleep_for(std::chrono::microseconds(AUDIO_THREAD_SLEEP_US));

                // optional: hammer other mutating calls that might be relevant
                // seq->move( seq->getTickPosition() ); // uncomment if you want more pressure
            }
        } catch (...) {
            audioThreadCrashed.store(true);
            // rethrowing here would abort test process; we mark the crash and exit loop
        }
    });

    std::thread mainThread([&]() {
        try {
            auto start = std::chrono::steady_clock::now();
            while (!stop.load()) {
                mpc.screens->get<SequencerScreen>()->update(nullptr, "");

                std::this_thread::sleep_for(std::chrono::microseconds(NOTIFY_THREAD_SLEEP_US));
            }
        } catch (...) {
            notifyThreadCrashed.store(true);
        }
    });

    // run for DURATION_MS
    std::this_thread::sleep_for(std::chrono::milliseconds(DURATION_MS));
    stop.store(true);

    audioThread.join();
    mainThread.join();

    // If either thread observed an exception we fail the test so CI notices it
    REQUIRE(!audioThreadCrashed.load());
    REQUIRE(!notifyThreadCrashed.load());

    // At minimum we assert we didn't crash; additional correctness checks may be added.
}

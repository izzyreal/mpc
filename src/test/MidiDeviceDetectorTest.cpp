#include "audiomidi/MidiDeviceDetector.hpp"
#include "catch2/catch_test_macros.hpp"
#include <RtMidi.h>

#include <chrono>
#include <future>

namespace mpc::audiomidi
{
    struct MidiDeviceDetectorTestAccess
    {
        static void start(MidiDeviceDetector &detector,
                          std::function<void()> work)
        {
            detector.startWorker(std::move(work));
        }

        static bool running(const MidiDeviceDetector &detector)
        {
            return detector.running.load();
        }
    };
} // namespace mpc::audiomidi

namespace
{
    using namespace std::chrono_literals;
    using mpc::audiomidi::MidiDeviceDetector;
    using Access = mpc::audiomidi::MidiDeviceDetectorTestAccess;

    bool waitUntilStopped(const MidiDeviceDetector &detector)
    {
        const auto deadline = std::chrono::steady_clock::now() + 2s;
        while (Access::running(detector) &&
               std::chrono::steady_clock::now() < deadline)
        {
            std::this_thread::yield();
        }
        return !Access::running(detector);
    }
} // namespace

TEST_CASE("MIDI detector clears running after an early preflight return",
          "[midi-device-detector]")
{
    MidiDeviceDetector detector;
    std::atomic<int> starts{0};
    Access::start(detector,
                  [&]
                  {
                      ++starts;
                  });
    REQUIRE(waitUntilStopped(detector));
    REQUIRE(starts.load() == 1);
    Access::start(detector,
                  [&]
                  {
                      ++starts;
                  });
    REQUIRE(waitUntilStopped(detector));
    REQUIRE(starts.load() == 2);
}

TEST_CASE("MIDI detector catches initialization failure and can restart",
          "[midi-device-detector]")
{
    MidiDeviceDetector detector;
    std::atomic<int> attempts{0};
    Access::start(detector,
                  [&]
                  {
                      ++attempts;
                      throw RtMidiError("Injected initialization failure",
                                        RtMidiError::DRIVER_ERROR);
                  });
    REQUIRE(waitUntilStopped(detector));
    REQUIRE(attempts.load() == 1);
    Access::start(detector,
                  [&]
                  {
                      ++attempts;
                  });
    REQUIRE(waitUntilStopped(detector));
    REQUIRE(attempts.load() == 2);
}

TEST_CASE("Starting a running MIDI detector does not replace its worker",
          "[midi-device-detector]")
{
    MidiDeviceDetector detector;
    std::promise<void> release;
    auto released = release.get_future().share();
    std::atomic<int> starts{0};
    Access::start(detector,
                  [&]
                  {
                      ++starts;
                      released.wait_for(2s);
                  });
    Access::start(detector,
                  [&]
                  {
                      ++starts;
                  });
    release.set_value();
    REQUIRE(waitUntilStopped(detector));
    REQUIRE(starts.load() == 1);
}

TEST_CASE("MIDI detector joins a stopped worker before starting another",
          "[midi-device-detector]")
{
    MidiDeviceDetector detector;
    std::promise<void> releaseOld, releaseNew;
    auto oldReleased = releaseOld.get_future().share();
    auto newReleased = releaseNew.get_future().share();
    std::atomic<bool> oldExited{false}, newSawOldExit{false};
    Access::start(detector,
                  [&]
                  {
                      oldReleased.wait_for(2s);
                      oldExited.store(true);
                  });
    detector.stop();
    auto restart =
        std::async(std::launch::async,
                   [&]
                   {
                       Access::start(detector,
                                     [&]
                                     {
                                         newSawOldExit.store(oldExited.load());
                                         newReleased.wait_for(2s);
                                     });
                   });
    const auto beforeRelease = restart.wait_for(20ms);
    releaseOld.set_value();
    restart.get();
    const bool newRunning = Access::running(detector);
    releaseNew.set_value();
    REQUIRE(waitUntilStopped(detector));
    REQUIRE(beforeRelease == std::future_status::timeout);
    REQUIRE(newRunning);
    REQUIRE(newSawOldExit.load());
}

TEST_CASE("MIDI detector destruction stops and joins its worker",
          "[midi-device-detector]")
{
    std::atomic<bool> exited{false};
    auto detector = std::make_unique<MidiDeviceDetector>();
    const auto *raw = detector.get();
    Access::start(*detector,
                  [&]
                  {
                      const auto deadline =
                          std::chrono::steady_clock::now() + 2s;
                      while (Access::running(*raw) &&
                             std::chrono::steady_clock::now() < deadline)
                      {
                          std::this_thread::yield();
                      }
                      exited.store(true);
                  });
    detector.reset();
    REQUIRE(exited.load());
}

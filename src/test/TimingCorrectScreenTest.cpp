#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>
#include <array>
#include <cstdlib>

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "TestMpc.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;

namespace
{
    void focusFieldByName(const std::shared_ptr<ScreenComponent> &screen,
                          const std::string &fieldName)
    {
        for (int i = 0; i < 16; ++i)
        {
            if (screen->getFocusedFieldNameOrThrow() == fieldName)
            {
                return;
            }
            screen->down();
        }
    }
} // namespace

TEST_CASE("TimingCorrectScreen", "[timing-correct-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    sequencer->getSelectedSequence()->init(1);
    stateManager->drainQueue();

    EventData eventData;
    eventData.type = EventType::NoteOn;
    eventData.tick = 1;
    eventData.noteNumber = mpc::NoteNumber(60);
    eventData.velocity = mpc::MaxVelocity;
    eventData.duration = mpc::Duration(1);

    auto tr = sequencer->getSelectedSequence()->getTrack(0);

    tr->acquireAndInsertEvent(eventData);

    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    auto controls = mpc.getScreen();
    controls->function(4); // DO IT
    stateManager->drainQueue();
    REQUIRE(tr->getNoteEvents().front()->getTick() == 0);

    tr->getNoteEvents().front()->setTick(1);

    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    controls->down();
    controls->down();
    controls->down();       // Move to 'Notes:' field.
    controls->turnWheel(1); // 'Notes:' was 'ALL', here we set it to '35/A01'.
    controls->function(4);  // DO IT
    stateManager->drainQueue();
    REQUIRE(tr->getNoteEvents().front()->getTick() == 1);
}

TEST_CASE("TimingCorrectScreen amount clamp follows Big time shift setting",
          "[timing-correct-screen]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();

    sequencer->getSelectedSequence()->init(1);
    stateManager->drainQueue();

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    auto timingScreen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
    auto controls = mpc.getScreen();

    controls->up();
    controls->up();
    controls->right();
    controls->down();
    REQUIRE(controls->getFocusedFieldNameOrThrow() == "amount");
    controls->turnWheel(500);
    REQUIRE(timingScreen->getAmount() == 11);

    mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcSettingsScreen);
    controls = mpc.getScreen();
    focusFieldByName(controls, "big-time-shift");
    controls->turnWheel(1);

    auto settingsScreen = mpc.screens->get<ScreenId::VmpcSettingsScreen>();
    REQUIRE(settingsScreen->isBigTimeShiftEnabled());

    mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    timingScreen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
    controls = mpc.getScreen();

    controls->up();
    controls->up();
    controls->right();
    controls->down();
    REQUIRE(controls->getFocusedFieldNameOrThrow() == "amount");
    controls->turnWheel(500);
    REQUIRE(timingScreen->getAmount() == 99);

    mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcSettingsScreen);
    controls = mpc.getScreen();
    focusFieldByName(controls, "big-time-shift");
    controls->turnWheel(-1);

    REQUIRE_FALSE(settingsScreen->isBigTimeShiftEnabled());
    REQUIRE(timingScreen->getAmount() == 11);
}

namespace
{
    struct TimingCorrectFixture
    {
        mpc::Mpc mpc;
        std::shared_ptr<Sequencer> sequencer;
        std::shared_ptr<SequencerStateManager> manager;
        std::shared_ptr<Sequence> sequence;
        std::shared_ptr<Track> track;
        std::shared_ptr<TimingCorrectScreen> screen;

        TimingCorrectFixture()
        {
            mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
            sequencer = mpc.getSequencer();
            manager = sequencer->getStateManager();
            sequence = sequencer->getSelectedSequence();
            sequence->init(1);
            manager->drainQueue();
            track = sequencer->getSelectedTrack();
            mpc.getLayeredScreen()->openScreenById(
                ScreenId::TimingCorrectScreen);
            screen = mpc.screens->get<ScreenId::TimingCorrectScreen>();
        }

        void wheel(const std::string &field, const int amount)
        {
            REQUIRE(mpc.getLayeredScreen()->setFocus(field));
            mpc.getScreen()->turnWheel(amount);
        }

        void configure(const int shift, const int swing = 50,
                       const int noteValue = 3)
        {
            screen->setNoteValue(noteValue);
            wheel("shifttiming", shift > 0 ? 1 : -1);
            wheel("amount", std::abs(shift));
            if (swing != 50)
            {
                wheel("swing", swing - 50);
            }
            REQUIRE(screen->getAmount() == std::abs(shift));
        }

        void addNote(const int tick, const int note = 60,
                     const int trackIndex = 0)
        {
            EventData data;
            data.type = EventType::NoteOn;
            data.tick = tick;
            data.noteNumber = mpc::NoteNumber(note);
            data.velocity = mpc::Velocity(100);
            data.duration = mpc::Duration(17);
            sequence->getTrack(trackIndex)->acquireAndInsertEvent(data);
        }

        std::vector<int> ticks(const int trackIndex = 0)
        {
            std::vector<int> result;
            for (const auto &event :
                 sequence->getTrack(trackIndex)->getNoteEvents())
            {
                result.push_back(event->getTick());
            }
            return result;
        }

        void apply()
        {
            manager->drainQueue();
            screen->function(4); // DO IT
            manager->drainQueue();
        }
    };
} // namespace

TEST_CASE("Timing Correct applies shift after quantization and swing with undo",
          "[timing-correct-screen]")
{
    const bool midi = GENERATE(false, true);
    const int shift = GENERATE(-5, 0, 5);
    const int swing = GENERATE(50, 75);
    CAPTURE(midi, shift, swing);
    TimingCorrectFixture f;
    f.track->setBusType(midi ? BusType::MIDI : BusType::DRUM1);
    f.configure(shift, swing);
    f.addNote(96);
    f.addNote(121);
    f.addNote(145);
    f.apply();

    const std::vector<int> expected{
        96 + shift, 120 + (swing == 75 ? 12 : 0) + shift, 144 + shift};
    REQUIRE(f.ticks() == expected);
    for (const auto &note : f.track->getNoteEvents())
    {
        REQUIRE(note->getDuration() == 17);
        REQUIRE(note->getVelocity() == 100);
    }
    f.sequencer->undoSeq();
    f.manager->drainQueue();
    REQUIRE(f.ticks() == std::vector<int>{96, 121, 145});
    f.sequencer->undoSeq();
    f.manager->drainQueue();
    REQUIRE(f.ticks() == expected);
}

TEST_CASE(
    "Timing Correct selects original ticks and notes on the selected track",
    "[timing-correct-screen]")
{
    const bool midi = GENERATE(false, true);
    const int shift = GENERATE(-5, 5);
    CAPTURE(midi, shift);
    TimingCorrectFixture f;
    f.track->setBusType(midi ? BusType::MIDI : BusType::DRUM1);
    f.manager->drainQueue();
    f.configure(shift);
    f.screen->setTime0(96);
    f.screen->setTime1(120);
    if (midi)
    {
        f.screen->setMidiNote0(mpc::NoteNumber(60));
        f.screen->setMidiNote1(mpc::NoteNumber(61));
    }
    else
    {
        f.screen->setDrumNote(mpc::DrumNoteNumber(60));
    }
    f.addNote(95, 59);
    f.addNote(95);
    f.addNote(96);
    f.addNote(119, midi ? 61 : 60);
    f.addNote(120);
    f.addNote(100, 62);
    f.addNote(101, 59);
    f.addNote(97, 60, 1);
    EventData control;
    control.type = EventType::ControlChange;
    control.tick = 97;
    f.track->acquireAndInsertEvent(control);
    f.apply();

    std::vector<int> expected{95, 95, 96 + shift, 120 + shift, 120, 100, 101};
    std::sort(expected.begin(), expected.end());
    REQUIRE(f.ticks() == expected);
    REQUIRE(f.ticks(1) == std::vector<int>{97});
    int controlCount = 0;
    for (const auto &event : f.track->getEvents())
    {
        if (event->snapshot.type == EventType::ControlChange)
        {
            ++controlCount;
            REQUIRE(event->getTick() == 97);
        }
    }
    REQUIRE(controlCount == 1);
}

TEST_CASE("Timing Correct may move notes outside the selected time range",
          "[timing-correct-screen]")
{
    TimingCorrectFixture f;
    f.configure(5);
    f.screen->setTime0(380);
    f.screen->setTime1(384);
    f.addNote(383);
    f.addNote(384, 61);
    f.apply();
    REQUIRE(f.ticks() == std::vector<int>{384, 389});
}

TEST_CASE("Timing Correct removes duplicates at their final shifted positions",
          "[timing-correct-screen]")
{
    TimingCorrectFixture f;
    f.configure(5);
    f.addNote(95);
    f.addNote(97);
    f.addNote(97, 61);
    f.apply();
    REQUIRE(f.ticks() == std::vector<int>{101, 101});
    f.sequencer->undoSeq();
    f.manager->drainQueue();
    REQUIRE(f.ticks() == std::vector<int>{95, 97, 97});
}

TEST_CASE("Timing Correct and live recording agree at sequence boundaries",
          "[timing-correct-screen]")
{
    // Input tick, note value, swing, signed shift, expected final tick.
    const auto [tick, noteValue, swing, shift, expected] =
        GENERATE((std::array<int, 5>{1, 3, 50, -5, 0}),
                 (std::array<int, 5>{145, 3, 50, 5, 149}),
                 (std::array<int, 5>{121, 3, 75, -5, 127}),
                 (std::array<int, 5>{121, 3, 75, 5, 137}),
                 (std::array<int, 5>{767, 3, 50, -5, 0}),
                 (std::array<int, 5>{767, 3, 50, 5, 5}),
                 (std::array<int, 5>{744, 3, 75, 99, 768}),
                 (std::array<int, 5>{145, 0, 50, 0, 145}),
                 (std::array<int, 5>{145, 0, 50, 99, 244}),
                 (std::array<int, 5>{767, 0, 50, 99, 768}));
    CAPTURE(tick, noteValue, swing, shift, expected);
    TimingCorrectFixture f;
    f.mpc.getLayeredScreen()->openScreenById(ScreenId::VmpcSettingsScreen);
    focusFieldByName(f.mpc.getScreen(), "big-time-shift");
    f.mpc.getScreen()->turnWheel(1);
    REQUIRE(f.mpc.screens->get<ScreenId::VmpcSettingsScreen>()
                ->isBigTimeShiftEnabled());
    f.mpc.getLayeredScreen()->openScreenById(ScreenId::TimingCorrectScreen);
    f.configure(shift, swing, noteValue);
    f.addNote(tick);
    f.apply();
    REQUIRE(f.ticks() == std::vector<int>{expected});

    auto *liveNote = f.manager->recordNoteEventLive(
        f.sequence->getSequenceIndex(), mpc::TrackIndex(1), mpc::NoteNumber(60),
        mpc::Velocity(100));
    f.manager->processLiveNoteEventRecordingQueues(
        tick, f.sequence->getSnapshot(f.sequence->getSequenceIndex()));
    REQUIRE(liveNote->tick == expected);
    f.manager->finalizeNoteEventLive(liveNote, tick + 1);
    f.manager->processLiveNoteEventRecordingQueues(
        tick + 1, f.sequence->getSnapshot(f.sequence->getSequenceIndex()));
    // Publish the changes made directly by the live recording queue.
    f.sequence->getTrack(1)->setOn(true, false);
    f.manager->drainQueue();
    REQUIRE(f.ticks(1) == std::vector<int>{expected});
}

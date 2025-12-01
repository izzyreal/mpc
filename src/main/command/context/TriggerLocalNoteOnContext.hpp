#pragma once

#include "IntTypes.hpp"

#include "performance/PerformanceEventSource.hpp"
#include "performance/EventTypes.hpp"

#include <memory>
#include <functional>
#include <optional>

namespace mpc::performance
{
    class PerformanceManager;
}

namespace mpc ::lcdgui
{
    class LayeredScreen;
    class ScreenComponent;
} // namespace mpc::lcdgui

namespace mpc::lcdgui::screens::window
{
    class TimingCorrectScreen;
    class Assign16LevelsScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::sampler
{
    class Program;
} // namespace mpc::sampler

namespace mpc::sequencer
{
    class Bus;
    class Sequencer;
    class Track;
} // namespace mpc::sequencer

namespace mpc::audiomidi
{
    class EventHandler;
} // namespace mpc::audiomidi

namespace mpc::command::context
{
    struct TriggerLocalNoteOnContext
    {
        performance::PerformanceEventSource source;
        std::weak_ptr<performance::PerformanceManager> performanceManager;
        performance::NoteOnEvent registryNoteOnEvent;
        bool isSequencerScreen;
        std::optional<ProgramPadIndex> programPadIndex;
        Velocity velocity;
        bool isFullLevelEnabled;
        bool isSixteenLevelsEnabled;
        bool isNoteRepeatLockedOrPressed;
        bool isErasePressed;
        bool isStepRecording;
        bool isRecMainWithoutPlaying;
        bool isRecordingOrOverdubbing;
        std::shared_ptr<sequencer::Bus> bus;

        std::shared_ptr<sampler::Program> program;
        NoteNumber note;

        sequencer::BusType drumScreenSelectedDrum;
        bool isSamplerScreen;

        sequencer::Track *track;
        std::weak_ptr<sequencer::Sequencer> sequencer;
        std::shared_ptr<lcdgui::screens::window::TimingCorrectScreen>
            timingCorrectScreen;
        std::shared_ptr<lcdgui::screens::window::Assign16LevelsScreen>
            assign16LevelsScreen;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;

        bool allowCentralNoteAndPadUpdate;
        std::shared_ptr<lcdgui::ScreenComponent> screenComponent;
        std::function<void(DrumNoteNumber)> setSelectedNote;
        std::function<void(ProgramPadIndex)> setSelectedPad;

        int hardwareSliderValue;
        Tick metronomeOnlyPositionTicks;
    };
} // namespace mpc::command::context

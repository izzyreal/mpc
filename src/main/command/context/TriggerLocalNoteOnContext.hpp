#pragma once

#include "IntTypes.hpp"

#include "performance/PerformanceEventSource.hpp"
#include "performance/EventTypes.hpp"

#include "controller/FnAlias.hpp"
#include "sequencer/Bus.hpp"

#include <optional>

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
    class Sequencer;
    class Track;
    class SequencerStateManager;
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
        sequencer::SequencerStateManager *sequencerStateManager;
        performance::NoteOnEvent registryNoteOnEvent;
        std::optional<PhysicalPadIndex> physicalPadIndex;
        std::optional<ProgramPadIndex> programPadIndex;
        Velocity velocity;
        bool isFullLevelEnabled;
        bool isSixteenLevelsEnabled;
        bool isStepRecording;
        bool isRecMainWithoutPlaying;
        bool isRecordingOrOverdubbing;
        sequencer::Bus *bus;

        sampler::Program *program;
        NoteNumber note;

        sequencer::BusType drumScreenSelectedDrum;
        bool isSamplerScreen;

        sequencer::Track *track;
        sequencer::Sequencer *sequencer;
        lcdgui::screens::window::TimingCorrectScreen *timingCorrectScreen;
        lcdgui::screens::window::Assign16LevelsScreen *assign16LevelsScreen;
        audiomidi::EventHandler *eventHandler;

        bool allowCentralNoteAndPadUpdate;
        lcdgui::ScreenComponent *screenComponent;
        const controller::SetSelectedNoteFn &setSelectedNote;
        const controller::SetSelectedPadFn &setSelectedPad;

        int hardwareSliderValue;
        Tick metronomeOnlyPositionTicks;
        Tick positionTicks;
    };
} // namespace mpc::command::context

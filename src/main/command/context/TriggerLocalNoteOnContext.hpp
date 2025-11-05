#pragma once

#include "eventregistry/EventRegistry.hpp"

#include <memory>
#include <functional>

namespace mpc::eventregistry
{
    class EventRegistry;
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
    class Sequencer;
    class Track;
    class FrameSeq;
} // namespace mpc::sequencer

namespace mpc::audiomidi
{
    class EventHandler;
} // namespace mpc::audiomidi

namespace mpc::command::context
{
    struct TriggerLocalNoteOnContext
    {
        eventregistry::Source source;
        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;
        eventregistry::NoteOnEvent *registryNoteOnEvent;
        bool isSequencerScreen;
        std::optional<int> programPadIndex;
        int velocity;
        bool isFullLevelEnabled;
        bool isSixteenLevelsEnabled;
        bool isNoteRepeatLockedOrPressed;
        bool isErasePressed;
        bool isStepRecording;
        bool isRecMainWithoutPlaying;
        bool isRecordingOrOverdubbing;
        std::shared_ptr<sequencer::Bus> bus;

        std::shared_ptr<mpc::sampler::Program> program;
        int note;

        int drumScreenSelectedDrum;
        bool isSamplerScreen;

        mpc::sequencer::Track *track;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen>
            timingCorrectScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::Assign16LevelsScreen>
            assign16LevelsScreen;
        std::shared_ptr<mpc::audiomidi::EventHandler> eventHandler;
        std::shared_ptr<mpc::sequencer::FrameSeq> frameSequencer;

        bool allowCentralNoteAndPadUpdate;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screenComponent;
        std::function<void(int)> setSelectedNote;
        std::function<void(int)> setSelectedPad;

        int hardwareSliderValue;
    };
} // namespace mpc::command::context

#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <functional>

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
    class Sampler;
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
    class AudioMidiServices;
    class EventHandler;
} // namespace mpc::audiomidi

namespace mpc::engine
{
    class PreviewSoundPlayer;
}

namespace mpc::command::context
{

    struct TriggerDrumNoteOnContext
    {
        bool isSequencerScreen;
        int programPadIndex;
        int velocity;
        bool isSoundScreen;

        bool isFullLevelEnabled;
        bool isSixteenLevelsEnabled;
        bool isNoteRepeatLockedOrPressed;
        bool isErasePressed;
        bool isStepRecording;
        bool isRecMainWithoutPlaying;
        bool isRecordingOrOverdubbing;
        int currentBarIndex;
        int64_t tickPosition;
        bool songModeEnabled;
        uint64_t metronomeOnlyTickPosition;
        int noteValueLengthInTicks;
        int swing;

        int trackBus;

        std::shared_ptr<mpc::sampler::Program> program;
        int note;

        int soundIndex;

        int drumScreenSelectedDrum;
        bool isSamplerScreen;

        std::shared_ptr<mpc::sequencer::Track> track;
        std::shared_ptr<mpc::sampler::Sampler> sampler;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::audiomidi::AudioMidiServices> audioMidiServices;
        std::shared_ptr<mpc::lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen>
            timingCorrectScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::Assign16LevelsScreen>
            assign16LevelsScreen;
        std::shared_ptr<mpc::audiomidi::EventHandler> eventHandler;
        std::shared_ptr<mpc::sequencer::FrameSeq> frameSequencer;

        mpc::engine::PreviewSoundPlayer &basicPlayer;

        bool allowCentralNoteAndPadUpdate;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screenComponent;
        std::function<void(int)> setSelectedNote;
        std::function<void(int)> setSelectedPad;

        std::string currentFieldName;
        int hardwareSliderValue;
        bool isProgramPadPressed;
    };
} // namespace mpc::command::context

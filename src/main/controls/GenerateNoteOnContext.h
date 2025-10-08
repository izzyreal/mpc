#pragma once

#include <memory>
#include <cstdint>

namespace mpc::sampler {
    class Program;
}

namespace mpc::sequencer {
    class Track;
    class Sequencer;
    class FrameSeq;
}

namespace mpc::lcdgui::screens::window {
    class TimingCorrectScreen;
    class Assign16LevelsScreen;
}

namespace mpc::audiomidi {
    class AudioMidiServices;
    class EventHandler;
}

namespace mpc::controls {

    class Controls;

    struct GenerateNoteOnContext {
        bool isSixteenLevelsEnabled;
        bool isRecordingOrOverdubbing;
        bool isStepRecording;
        bool isRecMainWithoutPlaying;
        int soundIndex;
        int trackBus;
        int currentBarIndex;
        int64_t tickPosition;
        int noteValueLengthInTicks;
        int swing;
        uint64_t metronomeOnlyTickPosition;
        int hardwareSliderValue;
        int drumScreenSelectedDrum;
        bool currentScreenIsSamplerScreen;
        std::shared_ptr<mpc::sampler::Program> program;
        std::shared_ptr<mpc::sequencer::Track> track;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::controls::Controls> controls;
        std::shared_ptr<mpc::audiomidi::AudioMidiServices> audioMidiServices;
        std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen> timingCorrectScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::Assign16LevelsScreen> assign16LevelsScreen;
        std::shared_ptr<mpc::audiomidi::EventHandler> eventHandler;
        std::shared_ptr<mpc::sequencer::FrameSeq> frameSequencer;
    };
}

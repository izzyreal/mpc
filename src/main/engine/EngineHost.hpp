#pragma once

#include "concurrency/SamplePreciseTaskQueue.hpp"
#include "concurrency/TaskQueue.hpp"

#include <memory>
#include <atomic>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::engine
{
    class NoteRepeatProcessor;
    class SequencerPlaybackEngine;
    class Voice;
    class MixerInterconnection;
    class PreviewSoundPlayer;
} // namespace mpc::engine

namespace mpc::engine::audio::core
{
    class AudioProcess;
}

namespace mpc::engine::audio::server
{
    class CompoundAudioClient;
    class NonRealTimeAudioServer;
    class RealTimeAudioServer;
    class AudioServer;
    class IOAudioProcess;
} // namespace mpc::engine::audio::server

namespace mpc::engine::audio::mixer
{
    class AudioMixer;
    class MixerControls;
} // namespace mpc::engine::audio::mixer

namespace mpc::audiomidi
{
    class MidiOutput;
    class DirectToDiskSettings;
    class DiskRecorder;
    class SoundRecorder;
    class SoundPlayer;
} // namespace mpc::audiomidi

namespace mpc::engine
{
    class EngineHost final
    {
        using NonRealTimeAudioServer = audio::server::NonRealTimeAudioServer;
        using RealTimeAudioServer = audio::server::RealTimeAudioServer;
        using CompoundAudioClient = audio::server::CompoundAudioClient;
        using AudioMixer = audio::mixer::AudioMixer;
        using MixerControls = audio::mixer::MixerControls;
        using AudioProcess = audio::core::AudioProcess;
        using IOAudioProcess = audio::server::IOAudioProcess;

    public:
        explicit EngineHost(Mpc &mpcToUse);

        void start();

        // Should be invoked on the audio thread at the very start of an
        // audio callback, in order to get the part of the model that is owned
        // by the audio thread in the correct state. This includes, but is not
        // limited to, the transport state, which pads are pressed and which
        // local note events are currently active.
        void prepareProcessBlock(int nFrames);

        utils::PostToAudioThreadFn postToAudioThread;
        void
        postSamplePreciseTaskToAudioThread(concurrency::SamplePreciseTask &);

        void flushNoteOffs();

        std::shared_ptr<SequencerPlaybackEngine> getSequencerPlaybackEngine();
        std::shared_ptr<NonRealTimeAudioServer> getAudioServer() const;
        std::shared_ptr<PreviewSoundPlayer> getPreviewSoundPlayer() const;
        std::shared_ptr<audio::mixer::AudioMixer> getMixer();
        std::vector<std::shared_ptr<Voice>> &getVoices();
        std::vector<MixerInterconnection *> &getMixerConnections();

        std::shared_ptr<audiomidi::SoundRecorder> getSoundRecorder();
        std::shared_ptr<audiomidi::SoundPlayer> getSoundPlayer();
        void setMainLevel(int i) const;
        int getMainLevel() const;
        void setMixerMasterLevel(int8_t dbValue) const;
        void setRecordLevel(int i) const;
        int getRecordLevel() const;
        void muteMonitor(bool mute) const;
        void initializeDiskRecorders();
        void closeIO() const;

        void connectVoices();
        void destroyServices() const;
        bool prepareBouncing(const audiomidi::DirectToDiskSettings *settings);
        bool isBouncePrepared() const;

        void finishPreviewSoundPlayerVoice() const;

    private:
        Mpc &mpc;
        std::atomic<bool> bouncing{false};
        std::atomic<bool> recordingSound{false};
        std::shared_ptr<AudioProcess> monitorInputAdapter;
        concurrency::TaskQueue audioTasks;
        concurrency::SamplePreciseTaskQueue preciseTasks;

    public:
        bool isBouncing() const;
        bool isRecordingSound() const;
        void startBouncing();
        void stopBouncing();
        void stopBouncingEarly();
        void startRecordingSound();
        void stopSoundRecorder(bool cancel = false);

        // Should be called from the audio thread only!
        void changeSoundRecorderStateIfRequired();

        // Should be called from the audio thread only!
        void changeBounceStateIfRequired();

        // Should be called from the audio thread only!
        void switchMidiControlMappingIfRequired() const;

    private:
        bool bouncePrepared = false;
        bool wasRecordingSound = false;
        bool wasBouncing = false;

        std::shared_ptr<PreviewSoundPlayer> previewSoundPlayer;
        std::shared_ptr<NoteRepeatProcessor> noteRepeatProcessor;
        std::shared_ptr<SequencerPlaybackEngine> sequencerPlaybackEngine;
        std::vector<std::shared_ptr<Voice>> voices;
        std::vector<MixerInterconnection *> mixerConnections;
        std::shared_ptr<Voice> basicVoice;
        std::shared_ptr<RealTimeAudioServer> realTimeAudioServer;
        std::shared_ptr<NonRealTimeAudioServer> nonRealTimeAudioServer;
        std::shared_ptr<AudioMixer> mixer;
        std::shared_ptr<MixerControls> mixerControls;
        std::shared_ptr<CompoundAudioClient> compoundAudioClient;
        std::shared_ptr<audiomidi::MidiOutput> midiOutput;
        std::shared_ptr<IOAudioProcess> inputProcess;
        std::vector<std::shared_ptr<IOAudioProcess>> outputProcesses;
        std::vector<std::shared_ptr<audiomidi::DiskRecorder>> diskRecorders;

        std::shared_ptr<audiomidi::SoundRecorder> soundRecorder;
        std::shared_ptr<audiomidi::SoundPlayer> soundPlayer;

        void setupMixer();
        void setAssignableMixOutLevels() const;
        void createSynth();
        void setMonitorLevel(int level) const;
    };
} // namespace mpc::engine

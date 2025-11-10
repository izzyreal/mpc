#pragma once

#include <memory>
#include <atomic>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::engine
{
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

namespace mpc::engine::audio::system
{
    class DefaultAudioSystem;
}

namespace mpc::audiomidi
{
    class MidiOutput;
    class DirectToDiskSettings;
    class DiskRecorder;
    class SoundRecorder;
    class SoundPlayer;
} // namespace mpc::audiomidi

namespace mpc::audiomidi
{

    class AudioMidiServices final
    {
        using NonRealTimeAudioServer =
            engine::audio::server::NonRealTimeAudioServer;
        using RealTimeAudioServer = engine::audio::server::RealTimeAudioServer;
        using CompoundAudioClient = engine::audio::server::CompoundAudioClient;
        using DefaultAudioSystem = engine::audio::system::DefaultAudioSystem;
        using AudioMixer = engine::audio::mixer::AudioMixer;
        using MixerControls = engine::audio::mixer::MixerControls;
        using AudioProcess = engine::audio::core::AudioProcess;
        using IOAudioProcess = engine::audio::server::IOAudioProcess;

    public:
        explicit AudioMidiServices(Mpc &mpcToUse);

        void start();

        std::shared_ptr<NonRealTimeAudioServer> getAudioServer() const;
        std::shared_ptr<engine::PreviewSoundPlayer>
        getPreviewSoundPlayer() const;
        std::shared_ptr<engine::audio::mixer::AudioMixer> getMixer();
        std::vector<std::shared_ptr<engine::Voice>> &getVoices();
        std::vector<engine::MixerInterconnection *> &getMixerConnections();

        std::shared_ptr<SoundRecorder> getSoundRecorder();
        std::shared_ptr<SoundPlayer> getSoundPlayer();
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
        bool prepareBouncing(const DirectToDiskSettings *settings);
        bool isBouncePrepared() const;

    private:
        Mpc &mpc;
        std::atomic<bool> bouncing{false};
        std::atomic<bool> recordingSound{false};
        std::shared_ptr<AudioProcess> monitorInputAdapter;

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

        std::shared_ptr<engine::PreviewSoundPlayer> previewSoundPlayer;
        std::vector<std::shared_ptr<engine::Voice>> voices;
        std::vector<engine::MixerInterconnection *> mixerConnections;
        std::shared_ptr<engine::Voice> basicVoice;
        std::shared_ptr<RealTimeAudioServer> realTimeAudioServer;
        std::shared_ptr<NonRealTimeAudioServer> nonRealTimeAudioServer;
        std::shared_ptr<DefaultAudioSystem> audioSystem;
        std::shared_ptr<AudioMixer> mixer;
        std::shared_ptr<MixerControls> mixerControls;
        std::shared_ptr<CompoundAudioClient> compoundAudioClient;
        std::shared_ptr<MidiOutput> midiOutput;
        std::shared_ptr<IOAudioProcess> inputProcess;
        std::vector<std::shared_ptr<IOAudioProcess>> outputProcesses;
        std::vector<std::shared_ptr<DiskRecorder>> diskRecorders;

        std::shared_ptr<SoundRecorder> soundRecorder;
        std::shared_ptr<SoundPlayer> soundPlayer;

        void setupMixer();
        void setAssignableMixOutLevels() const;
        void createSynth();
        void setMonitorLevel(int level) const;
    };
} // namespace mpc::audiomidi

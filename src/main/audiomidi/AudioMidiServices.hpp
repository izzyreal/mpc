#pragma once

#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/MixerInterconnection.hpp"
#include "engine/PreviewSoundPlayer.hpp"

#include <memory>
#include <atomic>

namespace mpc
{
    class Mpc;
}

namespace mpc::engine
{
    class Voice;
    class MixerInterconnection;
} // namespace mpc::engine

namespace mpc::engine::audio::core
{
    class AudioFormat;
}

namespace mpc::engine::audio::server
{
    class CompoundAudioClient;
    class NonRealTimeAudioServer;
    class AudioServer;
} // namespace mpc::engine::audio::server

namespace mpc::engine::audio::mixer
{
    class MixerControls;
}

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
    public:
        explicit AudioMidiServices(Mpc &mpcToUse);
        ~AudioMidiServices();
        void start();

        engine::audio::server::NonRealTimeAudioServer *getAudioServer() const;

        std::shared_ptr<SoundRecorder> getSoundRecorder();
        std::shared_ptr<SoundPlayer> getSoundPlayer();
        void setMainLevel(int i) const;
        int getMainLevel() const;
        void setMixerMasterLevel(int8_t dbValue) const;
        void setRecordLevel(int i) const;
        int getRecordLevel() const;
        void muteMonitor(bool mute) const;
        engine::PreviewSoundPlayer &getPreviewSoundPlayer() const;
        void initializeDiskRecorders();
        void closeIO() const;

        void connectVoices();
        void destroyServices() const;
        bool prepareBouncing(const DirectToDiskSettings *settings);
        bool isBouncePrepared() const;

        std::shared_ptr<engine::audio::mixer::AudioMixer> getMixer();
        std::vector<std::shared_ptr<engine::Voice>> &getVoices();
        std::vector<engine::MixerInterconnection *> &getMixerConnections();

    private:
        Mpc &mpc;
        std::atomic<bool> bouncing = ATOMIC_VAR_INIT(false);
        std::atomic<bool> recordingSound = ATOMIC_VAR_INIT(false);
        std::shared_ptr<engine::audio::core::AudioProcess> monitorInputAdapter;

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
        std::vector<std::shared_ptr<engine::control::CompoundControl>>
            soundPlayerChannelControls;
        std::unique_ptr<engine::PreviewSoundPlayer> basicSoundPlayerChannel;
        std::vector<std::shared_ptr<engine::Voice>> voices;
        std::vector<engine::MixerInterconnection *> mixerConnections;
        std::shared_ptr<engine::Voice> basicVoice;
        std::shared_ptr<engine::audio::server::AudioServer> server;
        std::shared_ptr<engine::audio::server::NonRealTimeAudioServer>
            offlineServer;
        std::shared_ptr<engine::audio::system::DefaultAudioSystem> audioSystem;
        std::shared_ptr<engine::audio::mixer::AudioMixer> mixer;
        std::shared_ptr<engine::audio::mixer::MixerControls> mixerControls;
        std::shared_ptr<engine::audio::server::CompoundAudioClient> cac;
        std::shared_ptr<MidiOutput> midiOutput;
        engine::audio::server::IOAudioProcess *inputProcess = nullptr;
        std::vector<engine::audio::server::IOAudioProcess *> outputProcesses;
        std::vector<std::shared_ptr<DiskRecorder>> diskRecorders;
        std::shared_ptr<SoundRecorder> soundRecorder;
        std::shared_ptr<SoundPlayer> soundPlayer;

        void setupMixer();
        void setAssignableMixOutLevels() const;
        void createSynth();
        void setMonitorLevel(int level) const;
    };
} // namespace mpc::audiomidi

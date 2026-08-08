#pragma once

#include "engine/audio/core/AudioProcess.hpp"
#include "hardware/ComponentId.hpp"

#include <array>
#include <atomic>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace mpc::engine
{
    enum class PhysicalSoundsMixMode : uint8_t
    {
        StereoOut = 0,
        Dedicated = 1
    };

    class PhysicalInteractionSoundPlayer final
        : public audio::core::AudioProcess
    {
    public:
        PhysicalInteractionSoundPlayer();

        void triggerButton(hardware::ComponentId componentId, bool isPress);
        void triggerPad(float normalizedVelocity);
        void triggerPowerOn();
        bool beginPowerOffRequest();
        void triggerPowerOff();
        bool isPowerOffComplete() const;
        double getPowerOffDurationSeconds() const;
        int processAudio(audio::core::AudioBuffer *buffer,
                         int nFrames) override;

        void setEnabled(bool shouldBeEnabled);
        bool isEnabled() const;
        void setLevel(int levelToUse);
        int getLevel() const;

        int getLoadedSampleCount() const;
        int getLoadedPadSampleCount() const;
        int getLoadedPowerSampleCount() const;

    private:
        struct Sample
        {
            std::vector<float> frames;
            int sampleRate = 48000;
        };

        struct Voice
        {
            const Sample *sample = nullptr;
            double position = 0.0;
        };

        static constexpr size_t ActionCount = 2;
        static constexpr size_t TakeCount = 2;
        static constexpr size_t PadVelocityLayerCount = 8;
        static constexpr size_t PadTakeCount = 6;
        static constexpr size_t PowerSampleCount = 2;
        static constexpr size_t MaxVoiceCount = 32;

        using Takes = std::array<Sample, TakeCount>;
        using Actions = std::array<Takes, ActionCount>;

        std::array<Actions, hardware::COMPONENT_ID_COUNT> samples;
        std::array<std::array<uint8_t, ActionCount>,
                   hardware::COMPONENT_ID_COUNT>
            nextTakes{};
        std::array<std::array<Sample, PadTakeCount>, PadVelocityLayerCount>
            padSamples;
        std::array<uint8_t, PadVelocityLayerCount> nextPadTakes{};
        std::array<Sample, PowerSampleCount> powerSamples;
        std::vector<Voice> voices;
        std::optional<Voice> lifecycleVoice;

        enum class LifecycleSound : uint8_t
        {
            None,
            PowerOn,
            PowerOff
        };

        enum class PowerOffState : uint8_t
        {
            Idle,
            Pending,
            Playing,
            Complete
        };

        LifecycleSound lifecycleSound = LifecycleSound::None;
        std::atomic<PowerOffState> powerOffState{PowerOffState::Idle};
        std::atomic<bool> enabled{true};
        std::atomic<int> level{100};
        int loadedSampleCount = 0;
        int loadedPadSampleCount = 0;
        int loadedPowerSampleCount = 0;

        void loadButtonSamples();
        void loadPadSamples();
        void loadPowerSamples();
        void addTransientVoice(const Sample &sample);
        static bool loadWavResource(const std::string &path, Sample &sample);
    };
} // namespace mpc::engine

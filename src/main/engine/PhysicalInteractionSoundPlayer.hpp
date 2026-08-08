#pragma once

#include "engine/audio/core/AudioProcess.hpp"
#include "hardware/ComponentId.hpp"

#include <array>
#include <atomic>
#include <cstdint>
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
        int processAudio(audio::core::AudioBuffer *buffer,
                         int nFrames) override;

        void setEnabled(bool shouldBeEnabled);
        bool isEnabled() const;
        void setLevel(int levelToUse);
        int getLevel() const;

        int getLoadedSampleCount() const;

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
        static constexpr size_t MaxVoiceCount = 32;

        using Takes = std::array<Sample, TakeCount>;
        using Actions = std::array<Takes, ActionCount>;

        std::array<Actions, hardware::COMPONENT_ID_COUNT> samples;
        std::array<std::array<uint8_t, ActionCount>,
                   hardware::COMPONENT_ID_COUNT>
            nextTakes{};
        std::vector<Voice> voices;
        std::atomic<bool> enabled{true};
        std::atomic<int> level{100};
        int loadedSampleCount = 0;

        void loadButtonSamples();
        static bool loadWavResource(const std::string &path, Sample &sample);
    };
} // namespace mpc::engine

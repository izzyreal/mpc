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

    enum class PhysicalSoundGroup : uint8_t
    {
        Buttons = 0,
        Pads,
        Slider,
        DataWheel,
        Power,
        Count
    };

    class PhysicalInteractionSoundPlayer final
        : public audio::core::AudioProcess
    {
    public:
        PhysicalInteractionSoundPlayer();

        void triggerButton(hardware::ComponentId componentId, bool isPress);
        void triggerPad(float normalizedVelocity);
        void triggerDataWheel(int steps, double inputTimeSeconds);
        void triggerSlider(float normalizedDelta, float normalizedPosition);
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
        void setGroupLevel(PhysicalSoundGroup group, int levelToUse);
        int getGroupLevel(PhysicalSoundGroup group) const;

        int getLoadedSampleCount() const;
        int getLoadedPadSampleCount() const;
        int getLoadedPowerSampleCount() const;
        int getLoadedDataWheelSampleCount() const;
        int getLoadedSliderSampleCount() const;

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
            float gain = 1.f;
            int startDelayFrames = 0;
            int fadeInSourceFrames = 0;
            int fadeOutSourceFrames = 0;
            bool sliderRub = false;
            bool dataWheelPhrase = false;
            PhysicalSoundGroup group = PhysicalSoundGroup::Buttons;
            int stopFadeFramesRemaining = -1;
            int stopFadeTotalFrames = 0;
        };

        enum class ButtonSoundGroup : uint8_t
        {
            Cursor = 0,
            Function,
            ModeBank,
            Locate,
            Screen,
            NoteTap,
            Numpad,
            Transport,
            UndoErase,
            Count
        };

        struct ShuffleBag
        {
            std::vector<size_t> order;
            size_t position = 0;
            std::optional<size_t> previous;
        };

        struct ButtonGroupSamples
        {
            std::vector<Sample> presses;
            std::vector<Sample> releases;
            ShuffleBag pressBag;
            ShuffleBag releaseBag;
        };

        static constexpr size_t ButtonSoundGroupCount =
            static_cast<size_t>(ButtonSoundGroup::Count);
        static constexpr size_t PadVelocityLayerCount = 8;
        static constexpr size_t PadTakeCount = 6;
        static constexpr size_t PowerSampleCount = 2;
        static constexpr size_t DataWheelDetentBankCount = 2;
        static constexpr size_t DataWheelTakeCount = 6;
        static constexpr size_t DataWheelFastPhraseCount = 6;
        static constexpr size_t SliderContactSampleCount = 19;
        static constexpr size_t SliderRubSampleCount = 5;
        static constexpr size_t SliderEndpointSampleCount = 2;
        static constexpr size_t MaxVoiceCount = 32;
        static constexpr size_t PhysicalSoundGroupCount =
            static_cast<size_t>(PhysicalSoundGroup::Count);

        std::array<ButtonGroupSamples, ButtonSoundGroupCount> buttonGroups;
        std::array<std::array<Sample, PadTakeCount>, PadVelocityLayerCount>
            padSamples;
        std::array<uint8_t, PadVelocityLayerCount> nextPadTakes{};
        std::array<Sample, PowerSampleCount> powerSamples;
        std::array<std::array<Sample, DataWheelTakeCount>,
                   DataWheelDetentBankCount>
            dataWheelDetentSamples;
        std::array<Sample, DataWheelFastPhraseCount> dataWheelFastPhraseSamples;
        std::array<Sample, SliderContactSampleCount> sliderContactSamples;
        std::array<Sample, SliderRubSampleCount> sliderRubSamples;
        std::array<Sample, SliderEndpointSampleCount> sliderEndpointSamples;
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
        std::atomic<bool> enabled{false};
        std::atomic<int> level{15};
        std::array<std::atomic<int>, PhysicalSoundGroupCount> groupLevels{};
        int loadedSampleCount = 0;
        int loadedPadSampleCount = 0;
        int loadedPowerSampleCount = 0;
        int loadedDataWheelSampleCount = 0;
        int loadedSliderSampleCount = 0;

        int wheelStepsRemaining = 0;
        int wheelDirection = 0;
        float wheelGestureRate = 8.f;
        double framesUntilWheelDetent = 0.0;
        std::array<uint8_t, DataWheelDetentBankCount> nextWheelDetents{};
        bool hasReceivedWheelInput = false;
        double lastWheelInputTimeSeconds = 0.0;
        double wheelFastMotionFramesRemaining = 0.0;
        double framesUntilWheelPhrase = 0.0;
        uint8_t nextWheelPhrase = 0;

        float pendingSliderDistance = 0.f;
        std::optional<size_t> pendingSliderEndpoint;
        uint64_t renderedFrameCount = 0;
        uint64_t lastSliderInputFrame = 0;
        uint64_t pendingSliderElapsedFrames = 0;
        bool hasReceivedSliderInput = false;
        double currentOutputSampleRate = 48000.0;
        double sliderMotionFramesRemaining = 0.0;
        double framesUntilSliderContact = 0.0;
        double framesUntilSliderRub = 0.0;
        float sliderSpeed = 0.f;
        float sliderHighSpeedBlend = 0.f;
        float sliderHighSpeedBlendTarget = 0.f;
        float sliderIntensity = 0.62f;
        uint8_t nextSliderContact = 0;
        uint8_t nextSliderRub = 0;
        uint32_t buttonRandomState = 0x42544e32U;
        uint32_t motionRandomState = 0x4d504332U;

        void loadButtonSamples();
        void loadPadSamples();
        void loadPowerSamples();
        void loadMotionSamples();
        void addTransientVoice(const Sample &sample, PhysicalSoundGroup group,
                               int startDelayFrames = 0, float voiceGain = 1.f,
                               bool sliderRub = false,
                               int fadeInSourceFrames = 0,
                               int fadeOutSourceFrames = 0,
                               bool dataWheelPhrase = false);
        void scheduleDataWheel(int outputFrameCount, double outputSampleRate);
        bool scheduleSlider(int outputFrameCount, double outputSampleRate);
        void requestDataWheelPhraseFade(int fadeFrames);
        void requestSliderRubFade(int fadeFrames);
        void resetMotionSchedulers();
        static std::optional<ButtonSoundGroup>
        buttonSoundGroupFor(hardware::ComponentId componentId);
        size_t drawButtonSampleIndex(ShuffleBag &bag, size_t sampleCount);
        uint32_t nextButtonRandom();
        float nextMotionRandomFloat();
        static bool loadWavResource(const std::string &path, Sample &sample);
    };
} // namespace mpc::engine

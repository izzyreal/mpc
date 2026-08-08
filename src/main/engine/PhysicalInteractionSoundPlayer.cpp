#include "engine/PhysicalInteractionSoundPlayer.hpp"

#include "Logger.hpp"
#include "MpcResourceUtil.hpp"
#include "engine/audio/core/AudioBuffer.hpp"

#include <algorithm>
#include <cmath>
#include <string>

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::engine::audio::core;
using namespace mpc::hardware;

namespace
{
    uint16_t readU16(const std::vector<char> &data, const size_t offset)
    {
        return static_cast<uint16_t>(
            static_cast<uint8_t>(data[offset]) |
            (static_cast<uint16_t>(static_cast<uint8_t>(data[offset + 1]))
             << 8));
    }

    uint32_t readU32(const std::vector<char> &data, const size_t offset)
    {
        return static_cast<uint32_t>(static_cast<uint8_t>(data[offset])) |
               (static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 1]))
                << 8) |
               (static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 2]))
                << 16) |
               (static_cast<uint32_t>(static_cast<uint8_t>(data[offset + 3]))
                << 24);
    }

    bool hasChunkId(const std::vector<char> &data, const size_t offset,
                    const char a, const char b, const char c, const char d)
    {
        return offset + 4 <= data.size() && data[offset] == a &&
               data[offset + 1] == b && data[offset + 2] == c &&
               data[offset + 3] == d;
    }
} // namespace

PhysicalInteractionSoundPlayer::PhysicalInteractionSoundPlayer()
{
    voices.reserve(MaxVoiceCount);
    loadButtonSamples();
    loadPadSamples();
    loadPowerSamples();
    loadMotionSamples();
}

void PhysicalInteractionSoundPlayer::loadButtonSamples()
{
    for (int component = CURSOR_LEFT_OR_DIGIT; component <= NUM_9_OR_MIDI_SYNC;
         ++component)
    {
        const auto componentId = static_cast<ComponentId>(component);
        const auto label = componentIdToLabel.find(componentId);
        if (label == componentIdToLabel.end())
        {
            continue;
        }

        for (size_t action = 0; action < ActionCount; ++action)
        {
            const std::string actionName = action == 0 ? "press" : "release";
            for (size_t take = 0; take < TakeCount; ++take)
            {
                const auto takeName = take == 0 ? "01" : "02";
                const auto path = "audio/physical-buttons/" + label->second +
                                  "_" + actionName + "_" + takeName + ".wav";
                if (loadWavResource(path, samples[component][action][take]))
                {
                    ++loadedSampleCount;
                }
                else
                {
                    MLOG("Unable to load physical interaction sound '" + path +
                         "'");
                }
            }
        }
    }
}

void PhysicalInteractionSoundPlayer::loadPadSamples()
{
    for (size_t layer = 0; layer < PadVelocityLayerCount; ++layer)
    {
        for (size_t take = 0; take < PadTakeCount; ++take)
        {
            const auto path =
                std::string("audio/physical-pads/pad_velocity_") +
                (layer < 9 ? "0" : "") + std::to_string(layer + 1) + "_take_" +
                (take < 9 ? "0" : "") + std::to_string(take + 1) + ".wav";
            if (loadWavResource(path, padSamples[layer][take]))
            {
                ++loadedSampleCount;
                ++loadedPadSampleCount;
            }
            else
            {
                MLOG("Unable to load physical interaction sound '" + path +
                     "'");
            }
        }
    }
}

void PhysicalInteractionSoundPlayer::loadPowerSamples()
{
    const std::array<std::string, PowerSampleCount> paths{
        "audio/physical-power/power_on.wav",
        "audio/physical-power/power_off.wav"};
    for (size_t i = 0; i < paths.size(); ++i)
    {
        if (loadWavResource(paths[i], powerSamples[i]))
        {
            ++loadedSampleCount;
            ++loadedPowerSampleCount;
        }
        else
        {
            MLOG("Unable to load physical interaction sound '" + paths[i] +
                 "'");
        }
    }
}

void PhysicalInteractionSoundPlayer::loadMotionSamples()
{
    const auto loadNumberedSamples =
        [this](auto &destination, const std::string &prefix, int &categoryCount)
    {
        for (size_t i = 0; i < destination.size(); ++i)
        {
            const auto number = (i < 9 ? "0" : "") + std::to_string(i + 1);
            const auto path =
                "audio/physical-motion/" + prefix + number + ".wav";
            if (loadWavResource(path, destination[i]))
            {
                ++loadedSampleCount;
                ++categoryCount;
            }
            else
            {
                MLOG("Unable to load physical interaction sound '" + path +
                     "'");
            }
        }
    };

    loadNumberedSamples(dataWheelSamples, "data-wheel-detent-",
                        loadedDataWheelSampleCount);
    loadNumberedSamples(sliderContactSamples, "slider-contact-",
                        loadedSliderSampleCount);
    loadNumberedSamples(sliderRubSamples, "slider-rub-",
                        loadedSliderSampleCount);

    const std::array<std::string, SliderEndpointSampleCount> endpointPaths{
        "audio/physical-motion/slider-endpoint-a.wav",
        "audio/physical-motion/slider-endpoint-b.wav"};
    for (size_t i = 0; i < endpointPaths.size(); ++i)
    {
        if (loadWavResource(endpointPaths[i], sliderEndpointSamples[i]))
        {
            ++loadedSampleCount;
            ++loadedSliderSampleCount;
        }
        else
        {
            MLOG("Unable to load physical interaction sound '" +
                 endpointPaths[i] + "'");
        }
    }
}

bool PhysicalInteractionSoundPlayer::loadWavResource(const std::string &path,
                                                     Sample &sample)
{
    const auto data = MpcResourceUtil::get_resource_data(path);
    if (data.size() < 44 || !hasChunkId(data, 0, 'R', 'I', 'F', 'F') ||
        !hasChunkId(data, 8, 'W', 'A', 'V', 'E'))
    {
        return false;
    }

    uint16_t audioFormat = 0;
    uint16_t channelCount = 0;
    uint16_t bitsPerSample = 0;
    uint32_t sampleRate = 0;
    size_t sampleDataOffset = 0;
    size_t sampleDataSize = 0;

    size_t offset = 12;
    while (offset + 8 <= data.size())
    {
        const auto chunkSize = static_cast<size_t>(readU32(data, offset + 4));
        const auto chunkDataOffset = offset + 8;
        if (chunkDataOffset > data.size() ||
            chunkSize > data.size() - chunkDataOffset)
        {
            return false;
        }

        if (hasChunkId(data, offset, 'f', 'm', 't', ' ') && chunkSize >= 16)
        {
            audioFormat = readU16(data, chunkDataOffset);
            channelCount = readU16(data, chunkDataOffset + 2);
            sampleRate = readU32(data, chunkDataOffset + 4);
            bitsPerSample = readU16(data, chunkDataOffset + 14);
        }
        else if (hasChunkId(data, offset, 'd', 'a', 't', 'a'))
        {
            sampleDataOffset = chunkDataOffset;
            sampleDataSize = chunkSize;
        }

        offset = chunkDataOffset + chunkSize + (chunkSize & 1U);
    }

    if (audioFormat != 1 || channelCount != 1 || bitsPerSample != 16 ||
        sampleRate == 0 || sampleDataOffset == 0 || sampleDataSize < 2)
    {
        return false;
    }

    sample.sampleRate = static_cast<int>(sampleRate);
    sample.frames.resize(sampleDataSize / 2);
    for (size_t i = 0; i < sample.frames.size(); ++i)
    {
        const auto raw = readU16(data, sampleDataOffset + i * 2);
        const auto signedSample = static_cast<int16_t>(raw);
        sample.frames[i] = static_cast<float>(signedSample) / 32768.f;
    }
    return true;
}

void PhysicalInteractionSoundPlayer::triggerButton(
    const ComponentId componentId, const bool isPress)
{
    if (!enabled.load(std::memory_order_relaxed) || !isButtonId(componentId))
    {
        return;
    }

    const auto component = static_cast<size_t>(componentId);
    const auto action = static_cast<size_t>(isPress ? 0 : 1);
    auto &nextTake = nextTakes[component][action];
    const auto &sample = samples[component][action][nextTake];
    nextTake = static_cast<uint8_t>((nextTake + 1) % TakeCount);

    if (sample.frames.empty())
    {
        return;
    }

    addTransientVoice(sample);
}

void PhysicalInteractionSoundPlayer::triggerPad(const float normalizedVelocity)
{
    if (!enabled.load(std::memory_order_relaxed) ||
        powerOffState.load(std::memory_order_relaxed) ==
            PowerOffState::Pending ||
        powerOffState.load(std::memory_order_relaxed) == PowerOffState::Playing)
    {
        return;
    }

    const auto velocity = std::clamp(normalizedVelocity, 0.f, 1.f);
    const auto layer =
        std::min(static_cast<size_t>(velocity * PadVelocityLayerCount),
                 PadVelocityLayerCount - 1);
    auto &nextTake = nextPadTakes[layer];
    const auto &sample = padSamples[layer][nextTake];
    nextTake = static_cast<uint8_t>((nextTake + 1) % PadTakeCount);
    addTransientVoice(sample);
}

void PhysicalInteractionSoundPlayer::triggerDataWheel(const int steps)
{
    if (!enabled.load(std::memory_order_relaxed) || steps == 0 ||
        powerOffState.load(std::memory_order_relaxed) ==
            PowerOffState::Pending ||
        powerOffState.load(std::memory_order_relaxed) == PowerOffState::Playing)
    {
        return;
    }

    const auto direction = steps > 0 ? 1 : -1;
    const auto stepCount = std::abs(steps);
    if (wheelStepsRemaining == 0)
    {
        framesUntilWheelDetent = 0.0;
    }
    else if (wheelDirection != direction)
    {
        // A reversal invalidates detents that represented motion in the old
        // direction but have not yet become audible.
        wheelStepsRemaining = 0;
        framesUntilWheelDetent = 0.0;
    }

    wheelDirection = direction;
    wheelStepsRemaining =
        std::min(12, wheelStepsRemaining + std::min(stepCount, 12));
    wheelDetentsPerSecond =
        static_cast<float>(std::clamp(18 + stepCount * 2, 18, 40));
}

void PhysicalInteractionSoundPlayer::triggerSlider(
    const float normalizedDelta, const float normalizedPosition)
{
    if (!enabled.load(std::memory_order_relaxed) ||
        powerOffState.load(std::memory_order_relaxed) ==
            PowerOffState::Pending ||
        powerOffState.load(std::memory_order_relaxed) == PowerOffState::Playing)
    {
        return;
    }

    const auto distance = std::abs(normalizedDelta);
    if (distance <= 0.f)
    {
        return;
    }

    if (hasReceivedSliderInput && renderedFrameCount != lastSliderInputFrame)
    {
        pendingSliderElapsedFrames += renderedFrameCount - lastSliderInputFrame;
    }
    else if (!hasReceivedSliderInput)
    {
        // A tenth of a second gives a useful velocity estimate for the first
        // update after an idle period without making large jumps sound slow.
        pendingSliderElapsedFrames =
            static_cast<uint64_t>(currentOutputSampleRate * 0.1);
        hasReceivedSliderInput = true;
    }
    lastSliderInputFrame = renderedFrameCount;
    pendingSliderDistance += distance;

    constexpr float EndpointTolerance = 1.0e-4f;
    if (normalizedDelta < 0.f && normalizedPosition <= EndpointTolerance)
    {
        // The audition's provisional A/B ordering is mapped A=top, B=bottom.
        pendingSliderEndpoint = 0;
    }
    else if (normalizedDelta > 0.f &&
             normalizedPosition >= 1.f - EndpointTolerance)
    {
        pendingSliderEndpoint = 1;
    }
}

void PhysicalInteractionSoundPlayer::triggerPowerOn()
{
    if (!enabled.load(std::memory_order_relaxed) ||
        powerSamples[0].frames.empty())
    {
        return;
    }

    lifecycleVoice = Voice{&powerSamples[0], 0.0};
    lifecycleSound = LifecycleSound::PowerOn;
}

bool PhysicalInteractionSoundPlayer::beginPowerOffRequest()
{
    if (!enabled.load(std::memory_order_relaxed) ||
        level.load(std::memory_order_relaxed) == 0 ||
        powerSamples[1].frames.empty())
    {
        return false;
    }

    auto expected = PowerOffState::Idle;
    if (powerOffState.compare_exchange_strong(expected, PowerOffState::Pending,
                                              std::memory_order_relaxed))
    {
        return true;
    }

    return expected == PowerOffState::Pending ||
           expected == PowerOffState::Playing;
}

void PhysicalInteractionSoundPlayer::triggerPowerOff()
{
    if (powerOffState.load(std::memory_order_relaxed) != PowerOffState::Pending)
    {
        return;
    }

    if (!enabled.load(std::memory_order_relaxed) ||
        level.load(std::memory_order_relaxed) == 0 ||
        powerSamples[1].frames.empty())
    {
        powerOffState.store(PowerOffState::Complete, std::memory_order_relaxed);
        return;
    }

    lifecycleVoice = Voice{&powerSamples[1], 0.0};
    lifecycleSound = LifecycleSound::PowerOff;
    powerOffState.store(PowerOffState::Playing, std::memory_order_relaxed);
}

bool PhysicalInteractionSoundPlayer::isPowerOffComplete() const
{
    return powerOffState.load(std::memory_order_relaxed) ==
           PowerOffState::Complete;
}

double PhysicalInteractionSoundPlayer::getPowerOffDurationSeconds() const
{
    const auto &sample = powerSamples[1];
    return sample.sampleRate > 0 ? static_cast<double>(sample.frames.size()) /
                                       static_cast<double>(sample.sampleRate)
                                 : 0.0;
}

void PhysicalInteractionSoundPlayer::addTransientVoice(
    const Sample &sample, const int startDelayFrames, const float voiceGain,
    const bool isSliderRub, const int fadeInSourceFrames,
    const int fadeOutSourceFrames)
{
    if (sample.frames.empty() ||
        powerOffState.load(std::memory_order_relaxed) ==
            PowerOffState::Pending ||
        powerOffState.load(std::memory_order_relaxed) == PowerOffState::Playing)
    {
        return;
    }

    if (voices.size() == MaxVoiceCount)
    {
        voices.erase(voices.begin());
    }
    Voice voice;
    voice.sample = &sample;
    voice.gain = voiceGain;
    voice.startDelayFrames = std::max(0, startDelayFrames);
    voice.fadeInSourceFrames = std::max(0, fadeInSourceFrames);
    voice.fadeOutSourceFrames = std::max(0, fadeOutSourceFrames);
    voice.sliderRub = isSliderRub;
    voices.push_back(std::move(voice));
}

float PhysicalInteractionSoundPlayer::nextMotionRandomFloat()
{
    motionRandomState = motionRandomState * 1664525U + 1013904223U;
    return static_cast<float>((motionRandomState >> 8U) & 0x00ffffffU) /
           static_cast<float>(0x01000000U);
}

void PhysicalInteractionSoundPlayer::scheduleDataWheel(
    const int outputFrameCount, const double outputSampleRate)
{
    if (wheelStepsRemaining <= 0 || loadedDataWheelSampleCount == 0)
    {
        return;
    }

    while (wheelStepsRemaining > 0 && framesUntilWheelDetent < outputFrameCount)
    {
        const auto &sample = dataWheelSamples[nextWheelDetent];
        nextWheelDetent =
            static_cast<uint8_t>((nextWheelDetent + 5) % DataWheelSampleCount);
        addTransientVoice(sample, static_cast<int>(framesUntilWheelDetent));
        --wheelStepsRemaining;
        if (wheelStepsRemaining > 0)
        {
            framesUntilWheelDetent +=
                outputSampleRate / std::max(4.f, wheelDetentsPerSecond);
        }
    }

    if (wheelStepsRemaining > 0)
    {
        framesUntilWheelDetent -= outputFrameCount;
    }
    else
    {
        framesUntilWheelDetent = 0.0;
        wheelDirection = 0;
    }
}

bool PhysicalInteractionSoundPlayer::scheduleSlider(
    const int outputFrameCount, const double outputSampleRate)
{
    if (pendingSliderDistance > 0.f)
    {
        const auto maximumElapsedFrames =
            static_cast<uint64_t>(outputSampleRate * 0.1);
        const auto elapsedFrames = std::max<uint64_t>(
            1, std::min(pendingSliderElapsedFrames > 0
                            ? pendingSliderElapsedFrames
                            : static_cast<uint64_t>(outputFrameCount),
                        maximumElapsedFrames));
        const auto instantaneousSpeed =
            static_cast<float>(pendingSliderDistance * outputSampleRate /
                               static_cast<double>(elapsedFrames));
        sliderSpeed = sliderMotionFramesRemaining > 0.0
                          ? sliderSpeed * 0.55f + instantaneousSpeed * 0.45f
                          : instantaneousSpeed;

        const auto blendPosition =
            std::clamp((sliderSpeed - 0.8f) / (2.4f - 0.8f), 0.f, 1.f);
        sliderHighSpeedBlendTarget =
            blendPosition * blendPosition * (3.f - 2.f * blendPosition);
        sliderIntensity =
            0.62f + 0.38f * std::clamp(sliderSpeed / 3.3f, 0.f, 1.f);

        const auto motionSeconds = std::clamp(
            pendingSliderDistance / std::max(sliderSpeed, 0.35f), 0.06f, 0.30f);
        sliderMotionFramesRemaining = std::max(
            sliderMotionFramesRemaining, motionSeconds * outputSampleRate);

        if (pendingSliderEndpoint &&
            *pendingSliderEndpoint < sliderEndpointSamples.size())
        {
            const auto endpointGain =
                0.35f + 0.65f * std::clamp(sliderSpeed / 3.3f, 0.f, 1.f);
            addTransientVoice(sliderEndpointSamples[*pendingSliderEndpoint], 0,
                              endpointGain);
        }

        pendingSliderDistance = 0.f;
        pendingSliderElapsedFrames = 0;
        pendingSliderEndpoint.reset();
    }

    if (sliderMotionFramesRemaining <= 0.0)
    {
        sliderHighSpeedBlend = 0.f;
        sliderHighSpeedBlendTarget = 0.f;
        return false;
    }

    const auto blendAlpha = static_cast<float>(
        1.0 - std::exp(-static_cast<double>(outputFrameCount) /
                       std::max(1.0, outputSampleRate * 0.05)));
    sliderHighSpeedBlend +=
        (sliderHighSpeedBlendTarget - sliderHighSpeedBlend) * blendAlpha;

    const auto activeFrameCount = static_cast<int>(
        std::min<double>(outputFrameCount, sliderMotionFramesRemaining));

    while (framesUntilSliderRub < activeFrameCount &&
           loadedSliderSampleCount > 0)
    {
        const auto &sample = sliderRubSamples[nextSliderRub];
        nextSliderRub = static_cast<uint8_t>(
            (nextSliderRub + 1 +
             static_cast<int>(nextMotionRandomFloat() *
                              (SliderRubSampleCount - 1))) %
            SliderRubSampleCount);
        const auto fadeFrames = static_cast<int>(sample.sampleRate * 0.060);
        addTransientVoice(sample, static_cast<int>(framesUntilSliderRub),
                          sliderIntensity * 0.45f, true, fadeFrames,
                          fadeFrames);
        const auto sampleDurationInOutputFrames =
            static_cast<double>(sample.frames.size()) * outputSampleRate /
            static_cast<double>(sample.sampleRate);
        framesUntilSliderRub += std::max(1.0, sampleDurationInOutputFrames -
                                                  outputSampleRate * 0.060);
    }
    framesUntilSliderRub -= outputFrameCount;

    if (sliderHighSpeedBlend > 0.01f)
    {
        const auto contactsPerSecond = 24.f + 48.f * sliderHighSpeedBlend;
        while (framesUntilSliderContact < activeFrameCount)
        {
            const auto &sample = sliderContactSamples[nextSliderContact];
            nextSliderContact = static_cast<uint8_t>(
                (nextSliderContact + 1 +
                 static_cast<int>(nextMotionRandomFloat() *
                                  (SliderContactSampleCount - 1))) %
                SliderContactSampleCount);
            const auto gainVariationDb = -1.8f + nextMotionRandomFloat() * 3.6f;
            const auto gainVariation = std::pow(10.f, gainVariationDb / 20.f);
            addTransientVoice(
                sample, static_cast<int>(framesUntilSliderContact),
                sliderIntensity * 0.67f * sliderHighSpeedBlend * gainVariation);
            const auto jitter = 0.78f + nextMotionRandomFloat() * 0.44f;
            framesUntilSliderContact +=
                outputSampleRate / contactsPerSecond * jitter;
        }
        framesUntilSliderContact -= outputFrameCount;
    }
    else
    {
        framesUntilSliderContact = 0.0;
    }

    const auto endsInThisBlock =
        sliderMotionFramesRemaining <= outputFrameCount;
    sliderMotionFramesRemaining =
        std::max(0.0, sliderMotionFramesRemaining - outputFrameCount);
    if (endsInThisBlock)
    {
        framesUntilSliderRub = 0.0;
        framesUntilSliderContact = 0.0;
        sliderHighSpeedBlendTarget = 0.f;
    }
    return endsInThisBlock;
}

void PhysicalInteractionSoundPlayer::requestSliderRubFade(const int fadeFrames)
{
    for (auto &voice : voices)
    {
        if (voice.sliderRub && voice.stopFadeFramesRemaining < 0)
        {
            voice.stopFadeFramesRemaining = std::max(1, fadeFrames);
            voice.stopFadeTotalFrames = std::max(1, fadeFrames);
        }
    }
}

void PhysicalInteractionSoundPlayer::resetMotionSchedulers()
{
    wheelStepsRemaining = 0;
    wheelDirection = 0;
    framesUntilWheelDetent = 0.0;
    pendingSliderDistance = 0.f;
    pendingSliderElapsedFrames = 0;
    pendingSliderEndpoint.reset();
    sliderMotionFramesRemaining = 0.0;
    framesUntilSliderContact = 0.0;
    framesUntilSliderRub = 0.0;
    sliderSpeed = 0.f;
    sliderHighSpeedBlend = 0.f;
    sliderHighSpeedBlendTarget = 0.f;
}

int PhysicalInteractionSoundPlayer::processAudio(AudioBuffer *buffer,
                                                 const int nFrames)
{
    buffer->makeSilence();

    const auto outputFrameCount = std::min(nFrames, buffer->getSampleCount());
    const auto outputSampleRate = static_cast<double>(buffer->getSampleRate());
    currentOutputSampleRate = outputSampleRate;

    if (!enabled.load(std::memory_order_relaxed))
    {
        voices.clear();
        lifecycleVoice.reset();
        lifecycleSound = LifecycleSound::None;
        resetMotionSchedulers();
        if (powerOffState.load(std::memory_order_relaxed) ==
                PowerOffState::Pending ||
            powerOffState.load(std::memory_order_relaxed) ==
                PowerOffState::Playing)
        {
            powerOffState.store(PowerOffState::Complete,
                                std::memory_order_relaxed);
        }
        renderedFrameCount += outputFrameCount;
        return AUDIO_OK;
    }

    const auto powerState = powerOffState.load(std::memory_order_relaxed);
    bool sliderStopsInThisBlock = false;
    if (powerState != PowerOffState::Pending &&
        powerState != PowerOffState::Playing)
    {
        scheduleDataWheel(outputFrameCount, outputSampleRate);
        sliderStopsInThisBlock =
            scheduleSlider(outputFrameCount, outputSampleRate);
    }
    else
    {
        resetMotionSchedulers();
        requestSliderRubFade(static_cast<int>(outputSampleRate * 0.030));
    }

    if (voices.empty() && !lifecycleVoice)
    {
        renderedFrameCount += outputFrameCount;
        return AUDIO_OK;
    }

    const auto gain =
        static_cast<float>(level.load(std::memory_order_relaxed)) * 0.01f;
    auto &left = buffer->getChannel(0);
    auto &right = buffer->getChannel(1);

    constexpr double HalfPi = 1.57079632679489661923;

    for (auto &voice : voices)
    {
        const auto &source = voice.sample->frames;
        const auto increment =
            static_cast<double>(voice.sample->sampleRate) / outputSampleRate;

        if (voice.startDelayFrames >= outputFrameCount)
        {
            voice.startDelayFrames -= outputFrameCount;
            continue;
        }
        const auto startFrame = voice.startDelayFrames;
        voice.startDelayFrames = 0;

        for (int frame = startFrame; frame < outputFrameCount; ++frame)
        {
            const auto sourceIndex = static_cast<size_t>(voice.position);
            if (sourceIndex >= source.size())
            {
                break;
            }

            const auto nextSourceIndex =
                std::min(sourceIndex + 1, source.size() - 1);
            const auto fraction = static_cast<float>(
                voice.position - static_cast<double>(sourceIndex));
            const auto value =
                (source[sourceIndex] +
                 (source[nextSourceIndex] - source[sourceIndex]) * fraction);

            float envelope = 1.f;
            if (voice.fadeInSourceFrames > 0 &&
                voice.position < voice.fadeInSourceFrames)
            {
                envelope *= static_cast<float>(std::sin(
                    HalfPi * voice.position / voice.fadeInSourceFrames));
            }
            const auto remainingSourceFrames =
                static_cast<double>(source.size()) - voice.position;
            if (voice.fadeOutSourceFrames > 0 &&
                remainingSourceFrames < voice.fadeOutSourceFrames)
            {
                envelope *=
                    static_cast<float>(std::sin(HalfPi * remainingSourceFrames /
                                                voice.fadeOutSourceFrames));
            }
            if (voice.stopFadeFramesRemaining >= 0)
            {
                if (voice.stopFadeFramesRemaining == 0)
                {
                    break;
                }
                envelope *= static_cast<float>(voice.stopFadeFramesRemaining) /
                            static_cast<float>(voice.stopFadeTotalFrames);
                --voice.stopFadeFramesRemaining;
            }

            const auto outputValue = value * gain * voice.gain * envelope;
            left[frame] += outputValue;
            right[frame] += outputValue;
            voice.position += increment;
        }
    }

    voices.erase(std::remove_if(voices.begin(), voices.end(),
                                [](const Voice &voice)
                                {
                                    return voice.position >=
                                               voice.sample->frames.size() ||
                                           voice.stopFadeFramesRemaining == 0;
                                }),
                 voices.end());

    if (lifecycleVoice)
    {
        auto &voice = *lifecycleVoice;
        const auto &source = voice.sample->frames;
        const auto increment =
            static_cast<double>(voice.sample->sampleRate) / outputSampleRate;

        for (int frame = 0; frame < outputFrameCount; ++frame)
        {
            const auto sourceIndex = static_cast<size_t>(voice.position);
            if (sourceIndex >= source.size())
            {
                break;
            }
            const auto nextSourceIndex =
                std::min(sourceIndex + 1, source.size() - 1);
            const auto fraction = static_cast<float>(
                voice.position - static_cast<double>(sourceIndex));
            const auto value =
                (source[sourceIndex] +
                 (source[nextSourceIndex] - source[sourceIndex]) * fraction) *
                gain;
            left[frame] += value;
            right[frame] += value;
            voice.position += increment;
        }

        if (voice.position >= source.size())
        {
            const auto completedSound = lifecycleSound;
            lifecycleVoice.reset();
            lifecycleSound = LifecycleSound::None;
            if (completedSound == LifecycleSound::PowerOff)
            {
                powerOffState.store(PowerOffState::Complete,
                                    std::memory_order_relaxed);
            }
        }
    }

    if (sliderStopsInThisBlock)
    {
        requestSliderRubFade(static_cast<int>(outputSampleRate * 0.030));
    }
    renderedFrameCount += outputFrameCount;
    return AUDIO_OK;
}

void PhysicalInteractionSoundPlayer::setEnabled(const bool shouldBeEnabled)
{
    enabled.store(shouldBeEnabled, std::memory_order_relaxed);
}

bool PhysicalInteractionSoundPlayer::isEnabled() const
{
    return enabled.load(std::memory_order_relaxed);
}

void PhysicalInteractionSoundPlayer::setLevel(const int levelToUse)
{
    level.store(std::clamp(levelToUse, 0, 100), std::memory_order_relaxed);
}

int PhysicalInteractionSoundPlayer::getLevel() const
{
    return level.load(std::memory_order_relaxed);
}

int PhysicalInteractionSoundPlayer::getLoadedSampleCount() const
{
    return loadedSampleCount;
}

int PhysicalInteractionSoundPlayer::getLoadedPadSampleCount() const
{
    return loadedPadSampleCount;
}

int PhysicalInteractionSoundPlayer::getLoadedPowerSampleCount() const
{
    return loadedPowerSampleCount;
}

int PhysicalInteractionSoundPlayer::getLoadedDataWheelSampleCount() const
{
    return loadedDataWheelSampleCount;
}

int PhysicalInteractionSoundPlayer::getLoadedSliderSampleCount() const
{
    return loadedSliderSampleCount;
}

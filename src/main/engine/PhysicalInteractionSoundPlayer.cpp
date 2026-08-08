#include "engine/PhysicalInteractionSoundPlayer.hpp"

#include "Logger.hpp"
#include "MpcResourceUtil.hpp"
#include "engine/audio/core/AudioBuffer.hpp"

#include <algorithm>
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

void PhysicalInteractionSoundPlayer::addTransientVoice(const Sample &sample)
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
    voices.push_back(Voice{&sample, 0.0});
}

int PhysicalInteractionSoundPlayer::processAudio(AudioBuffer *buffer,
                                                 const int nFrames)
{
    buffer->makeSilence();

    if (!enabled.load(std::memory_order_relaxed))
    {
        voices.clear();
        lifecycleVoice.reset();
        lifecycleSound = LifecycleSound::None;
        if (powerOffState.load(std::memory_order_relaxed) ==
                PowerOffState::Pending ||
            powerOffState.load(std::memory_order_relaxed) ==
                PowerOffState::Playing)
        {
            powerOffState.store(PowerOffState::Complete,
                                std::memory_order_relaxed);
        }
        return AUDIO_OK;
    }

    if (voices.empty() && !lifecycleVoice)
    {
        return AUDIO_OK;
    }

    const auto outputFrameCount = std::min(nFrames, buffer->getSampleCount());
    const auto outputSampleRate = static_cast<double>(buffer->getSampleRate());
    const auto gain =
        static_cast<float>(level.load(std::memory_order_relaxed)) * 0.01f;
    auto &left = buffer->getChannel(0);
    auto &right = buffer->getChannel(1);

    for (auto &voice : voices)
    {
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
    }

    voices.erase(std::remove_if(voices.begin(), voices.end(),
                                [](const Voice &voice)
                                {
                                    return voice.position >=
                                           voice.sample->frames.size();
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

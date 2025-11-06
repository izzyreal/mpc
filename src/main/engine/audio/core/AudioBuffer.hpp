#pragma once
#include "engine/audio/core/FloatSampleBuffer.hpp"

#include <string>

namespace mpc::engine::audio::core
{
    class AudioBuffer : public FloatSampleBuffer
    {

    private:
        bool realTime{true};
        std::string name;

    public:
        std::string getName();

    protected:
        void setChannelCount(int count);

    public:
        bool isRealTime() const;
        void setRealTime(bool realTime);

    public:
        void swap(int a, int b);
        float square();

        void copyFrom(AudioBuffer *src);
        const bool isSilent();

    public:
        AudioBuffer(const std::string &name, int channelCount, int sampleCount,
                    float sampleRate);
    };
} // namespace mpc::engine::audio::core

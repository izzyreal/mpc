#pragma once

#include <cstdint>
#include <engine/audio/core/Encoding.hpp>

namespace mpc::engine::audio::core {

    class AudioFormat final
    {

    public:
        AudioFormat(float sampleRate, int sampleSizeInBits, int channels, bool signed_, bool bigEndian);

        AudioFormat(Encoding *encoding, float sampleRate, int sampleSizeInBits, int channels, int frameSize,
                    float frameRate, bool bigEndian);

    protected:
        bool bigEndian = {false};
        int channels = {0};
        Encoding *encoding = {nullptr};
        float frameRate = {0};
        int frameSize = {0};

    private:
        const int NOT_SPECIFIED = -1;

    public:
        float sampleRate = {0};
        int sampleSizeInBits = {0};

    public:
        virtual int getChannels();

        virtual Encoding *getEncoding();

        virtual float getFrameRate();

        virtual int getFrameSize();

        virtual float getSampleRate();

        virtual int getSampleSizeInBits();

        virtual bool isBigEndian();

        virtual bool matches(AudioFormat *format);
    };

}

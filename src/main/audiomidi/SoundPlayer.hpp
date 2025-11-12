#pragma once
#include "engine/audio/core/AudioProcess.hpp"

#include "sampler/Sound.hpp"

#include <samplerate.h>

#include <memory>
#include <atomic>
#include <thread>

using namespace mpc::sampler;
using namespace mpc::engine::audio::core;

namespace moodycamel
{
    template <typename T, size_t MAX_BLOCK_SIZE> class ReaderWriterQueue;
}

namespace mpc::audiomidi
{
    enum SoundPlayerFileFormat
    {
        SND,
        WAV
    };

    class SoundPlayer : public AudioProcess
    {

        std::thread readThread;
        int ingestedSourceFrameCount = 0;
        int sourceFrameCount = 0;
        int playedFrameCount = 0;
        std::atomic_ulong totalResamplerGeneratedFrameCount = 0;
        unsigned long resamplerGeneratedFrameCounter = 0;
        std::shared_ptr<AudioFormat> inputAudioFormat;
        SoundPlayerFileFormat fileFormat;
        float fadeFactor = -1.0f;
        bool stopEarly = false;
        void readWithoutResampling();
        void readWithResampling(float ratio);
        short readNextShort() const;
        int32_t readNext24BitInt() const;
        float readNextFrame();

        std::atomic_bool playing = false;
        std::string filePath;
        std::shared_ptr<moodycamel::ReaderWriterQueue<float, 512>> bufferLeft;
        std::shared_ptr<moodycamel::ReaderWriterQueue<float, 512>> bufferRight;
        std::vector<float> resampleInputBufferLeft;
        std::vector<float> resampleInputBufferRight;
        std::vector<float> resampleOutputBuffer;
        SRC_STATE *srcLeft = nullptr;
        SRC_STATE *srcRight = nullptr;
        int srcLeftError = 0;
        int srcRightError = 0;
        std::shared_ptr<std::istream> stream;

    public:
        bool start(const std::shared_ptr<std::istream> &, SoundPlayerFileFormat,
                   int audioServerSampleRate);
        int processAudio(AudioBuffer *buf, int nFrames) override;
        void enableStopEarly();
        bool isPlaying() const;

        SoundPlayer();
        ~SoundPlayer();
    };
} // namespace mpc::audiomidi

#pragma once
#include "engine/audio/core/AudioProcess.hpp"

#include "sampler/Sound.hpp"
#include "audiomidi/MonoResampler.hpp"

#include <memory>
#include <atomic>
#include <vector>
#include <cstdint>

using namespace mpc::sampler;
using namespace mpc::engine::audio::core;

/*
 * A SoundRecorder always records at 44.1KHz, like the real MPC2000XL.
 * This implementation uses an always-writing lock-free circular buffer
 * (audio thread writes atomic writeIndex). When recording is triggered,
 * the reader index is set to (writeIndex - preRecFrames) and the audio
 * thread reads from the ring to produce the final sample — no locks.
 */

namespace mpc
{
    class Mpc;
}

namespace mpc::audiomidi
{
    class SoundRecorder final : public AudioProcess
    {
        Mpc &mpc;

        std::atomic<bool> recording{false};
        std::atomic<bool> cancelled{false};
        int lengthInFramesAtEngineSampleRate = 0;
        int recordedFrameCountAtEngineSampleRate = 0;

        // modes: 0 = MONO L, 1 = MONO R, 2 = STEREO
        int mode = 0;

        std::shared_ptr<Sound> sound;

        const size_t INTERNAL_BUF_SIZE = 100000;

        std::vector<MonoResampler> resamplers = std::vector<MonoResampler>(2);

        std::vector<float> leftChannelCopy;
        std::vector<float> rightChannelCopy;
        std::vector<float> unresampledLeft;
        std::vector<float> unresampledRight;
        std::vector<float> resampledLeft;
        std::vector<float> resampledRight;

        std::vector<float> ringLeft;
        std::vector<float> ringRight;
        size_t writeIndex = 0;
        size_t readIndex = 0;

        size_t preRecFramesAtEngineSampleRate = 0;

        int engineSampleRate = 44100;

        std::atomic<int8_t> inputGain{0};
        std::atomic<bool> sampleScreenActive{false};
        std::atomic<bool> armed{false};

    public:
        explicit SoundRecorder(Mpc &mpc);

        void setSampleScreenActive(bool active);
        void prepare(const std::shared_ptr<Sound> &, int newLengthInFrames,
                     int engineSampleRate);
        void start();
        void stop();
        void cancel();
        int processAudio(AudioBuffer *buf, int nFrames) override;
        bool isRecording() const;
        int8_t getInputGain() const;
        void setInputGain(int8_t);
        void setArmed(bool b);
        bool isArmed() const;

        void open() override {}
        void close() override {}
    };
} // namespace mpc::audiomidi

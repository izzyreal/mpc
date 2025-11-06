#pragma once
#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

#include "MonoResampler.hpp"

#include "circular_buffer.hpp"

#include <Observer.hpp>

#include <memory>
#include <atomic>

using namespace mpc::sampler;
using namespace mpc::engine::audio::core;

/*
 * A SoundRecorder always records at 44.1KHz, like the real MPC2000XL.
 */

namespace mpc
{
    class Mpc;
}

namespace mpc::audiomidi
{
    class SoundRecorder final : public AudioProcess, public Observable
    {
        Mpc &mpc;
        bool recording = false;
        bool cancelled = false;
        int lengthInFramesAtEngineSampleRate = 0;

        // modes: 0 = MONO L, 1 = MONO R, 2 = STEREO
        int mode = 0;

        std::shared_ptr<Sound> sound;

        const uint32_t INTERNAL_BUF_SIZE = 100000;

        std::vector<MonoResampler> resamplers = std::vector<MonoResampler>(2);

        std::vector<float> leftChannelCopy =
            std::vector<float>(INTERNAL_BUF_SIZE);
        std::vector<float> rightChannelCopy =
            std::vector<float>(INTERNAL_BUF_SIZE);
        circular_buffer<float> ringBufferLeft =
            circular_buffer<float>(INTERNAL_BUF_SIZE);
        circular_buffer<float> ringBufferRight =
            circular_buffer<float>(INTERNAL_BUF_SIZE);
        std::vector<float> unresampledLeft =
            std::vector<float>(INTERNAL_BUF_SIZE);
        std::vector<float> unresampledRight =
            std::vector<float>(INTERNAL_BUF_SIZE);
        std::vector<float> resampledLeft =
            std::vector<float>(INTERNAL_BUF_SIZE);
        std::vector<float> resampledRight =
            std::vector<float>(INTERNAL_BUF_SIZE);

        int recordedFrameCountAtEngineSampleRate = 0;
        int engineSampleRate = 44100;

        unsigned int inputGain = 0;
        std::atomic<bool> sampleScreenActive = ATOMIC_VAR_INIT(false);
        bool lastSampleScreenActive = false;
        bool armed = false;

    public:
        void setSampleScreenActive(bool active);
        void prepare(const std::shared_ptr<Sound> &, int newLengthInFrames,
                     int engineSampleRate);
        void start();
        void stop();
        void cancel();
        int processAudio(mpc::engine::audio::core::AudioBuffer *buf,
                         int nFrames) override;
        bool isRecording() const;
        unsigned int getInputGain() const;
        void setInputGain(unsigned int);
        void setArmed(bool b);
        bool isArmed() const;

        SoundRecorder(mpc::Mpc &mpc);

        void open() override {}
        void close() override {}
    };
} // namespace mpc::audiomidi

#pragma once
#include <engine/audio/core/AudioProcess.hpp>

#include "sampler/Sound.hpp"
#include "MonoResampler.hpp"

#include <memory>
#include <atomic>
#include <vector>

using namespace mpc::sampler;
using namespace mpc::engine::audio::core;

/*
 * A SoundRecorder always records at 44.1KHz, like the real MPC2000XL.
 * This implementation uses an always-writing lock-free circular buffer
 * (audio thread writes atomic writeIndex). When recording is triggered,
 * the reader index is set to (writeIndex - preRecFrames) and the audio
 * thread reads from the ring to produce the final sample — no locks.
 */

namespace mpc { class Mpc; }

namespace mpc::audiomidi
{
    class SoundRecorder final : public AudioProcess
    {
        Mpc &mpc;
        std::atomic<bool> processing{false}; // true while audio thread is inside processAudio

        // Recording state
        std::atomic<bool> recording{false}; // set/reset on audio thread
        bool cancelled = false;
        int lengthInFramesAtEngineSampleRate = 0;
        int recordedFrameCountAtEngineSampleRate = 0;

        // modes: 0 = MONO L, 1 = MONO R, 2 = STEREO
        int mode = 0;

        std::shared_ptr<Sound> sound;

        // internal ring buffer size (frames)
        const size_t INTERNAL_BUF_SIZE = 100000;

        // resamplers / helpers
        std::vector<MonoResampler> resamplers = std::vector<MonoResampler>(2);

        // scratch arrays sized to BUFFER callback max
        std::vector<float> leftChannelCopy;
        std::vector<float> rightChannelCopy;
        std::vector<float> unresampledLeft;
        std::vector<float> unresampledRight;
        std::vector<float> resampledLeft;
        std::vector<float> resampledRight;

        std::vector<float> ringLeft;
        std::vector<float> ringRight;
        std::atomic<size_t> writeIndex{0};
        size_t readIndex = 0;

        size_t preRecFramesAtEngineSampleRate = 0;

        int engineSampleRate = 44100;

        unsigned int inputGain = 0;
        std::atomic<bool> sampleScreenActive{false};
        bool lastSampleScreenActive = false;
        bool armed = false;

    public:
        explicit SoundRecorder(Mpc &mpc);

        void setSampleScreenActive(bool active);
        void prepare(const std::shared_ptr<Sound> &, int newLengthInFrames, int engineSampleRate);
        void start(); // must be called on audio thread (sets readIndex using writeIndex)
        void stop();  // called on UI thread to stop recording early
        void cancel();
        int processAudio(AudioBuffer *buf, int nFrames) override;
        bool isRecording() const;
        unsigned int getInputGain() const;
        void setInputGain(unsigned int);
        void setArmed(bool b);
        bool isArmed() const;

        void open() override {}
        void close() override {}
    };
} // namespace mpc::audiomidi

#pragma once
#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioProcess.hpp>

#include <sampler/Sound.hpp>

#include <collections/TSCircularBuffer.hpp>
#include <thirdp/libsamplerate/samplerate.h>

#include <fstream>
#include <memory>
#include <atomic>
#include <mutex>

using namespace mpc::sampler;
using namespace mpc::engine::audio::core;

namespace mpc::audiomidi
{
enum SoundPlayerFileFormat { SND, WAV };

class SoundPlayer
: public AudioProcess
{
    
private:
    int ingestedSourceFrameCount = 0;
    int sourceFrameCount = 0;
    std::shared_ptr<AudioFormat> audioFormat;
    SoundPlayerFileFormat fileFormat;
    float fadeFactor = -1.0f;
    bool stopEarly = false;
    
private:
    std::mutex _playing;
    bool playing = false;
    std::string filePath;
    circular_buffer<float> resampleInputBufferLeft = circular_buffer<float>(60000);
    circular_buffer<float> resampleInputBufferRight = circular_buffer<float>(60000);
    circular_buffer<float> resampleOutputBufferLeft = circular_buffer<float>(60000);
    circular_buffer<float> resampleOutputBufferRight = circular_buffer<float>(60000);
    SRC_STATE* srcLeft = nullptr;
    SRC_STATE* srcRight = nullptr;
    int srcLeftError = 0;
    int srcRightError = 0;
    std::shared_ptr<std::istream> stream;
    
public:
    bool start(std::shared_ptr<std::istream>, SoundPlayerFileFormat);
    int processAudio(mpc::engine::audio::core::AudioBuffer* buf, int nFrames) override;
    void open() override {}
    void close() override {}
    
public:
    void enableStopEarly();
    bool isPlaying();
    
private:
    void stop();
    void resampleChannel(bool left, std::vector<float>& input, int sourceSampleRate, int destinationSampleRate, bool endOfInput);
    
public:
    SoundPlayer();
    ~SoundPlayer();
    
};
}

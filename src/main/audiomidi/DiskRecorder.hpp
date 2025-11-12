#pragma once

#include "mpc_fs.hpp"

#include "engine/audio/core/AudioProcessAdapter.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <thread>

#ifdef __linux__
#include <atomic>
#endif

namespace mpc
{
    class Mpc;
}

namespace mpc::engine::audio::core
{
    class AudioProcess;
}

namespace moodycamel
{
    template <typename T, size_t MAX_BLOCK_SIZE> class ReaderWriterQueue;
}

namespace mpc::audiomidi
{
    class DiskRecorder : public engine::audio::core::AudioProcessAdapter

    {
    public:
        DiskRecorder(Mpc &, AudioProcess *, int index);

        ~DiskRecorder() override;

    private:
        Mpc &mpc;
        const int BUFFER_SIZE =
            192000; // Number of frames for 1 second at 192khz
        std::thread writeThread;
        std::shared_ptr<moodycamel::ReaderWriterQueue<float, 512>>
            ringBufferLeft;
        std::shared_ptr<moodycamel::ReaderWriterQueue<float, 512>>
            ringBufferRight;
        std::vector<float> bufferLeft;
        std::vector<float> bufferRight;

        std::vector<char> byteBufferLeft;
        std::vector<char> byteBufferRight;
        std::vector<char> stereoByteBuffer;

        static const std::vector<std::pair<std::string, std::string>>
            fileNamesMono;
        static const std::vector<std::string> fileNamesStereo;

        int index = 0;
        fs::path destinationDirectory;
        engine::audio::core::AudioFormat *outputFileFormat = nullptr;
        std::vector<std::ofstream> fileStreams;
        std::atomic<bool> writing{false};
        std::atomic<bool> preparedToWrite{false};
        int writtenByteCount = 0;
        int lengthInFrames = 0;
        int lengthInBytes = 0;
        bool isOnlySilence = true;

        void writeRingBufferToDisk();
        void removeFilesIfEmpty() const;

    public:
        bool start();
        bool stopEarly();
        bool prepare(int lengthInFrames, int sampleRate, bool isStereo,
                     fs::path destinationDirectory);

        int processAudio(engine::audio::core::AudioBuffer *,
                         int nFrames) override;
    };
} // namespace mpc::audiomidi

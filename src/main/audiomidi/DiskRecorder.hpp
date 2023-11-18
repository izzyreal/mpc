#pragma once

#include "../mpc_fs.hpp"

#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioProcess.hpp>

#include <readerwriterqueue.h>

#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <thread>

#ifdef __linux__
#include <atomic>
#endif

namespace mpc { class Mpc; }

namespace mpc::audiomidi {

	class DiskRecorder
		: public mpc::engine::audio::core::AudioProcessAdapter

	{

	private:
        mpc::Mpc& mpc;
        const int BUFFER_SIZE = 192000; // Number of frames for 1 second at 192khz
        std::thread writeThread;
        moodycamel::ReaderWriterQueue<float> ringBufferLeft = moodycamel::ReaderWriterQueue<float>(BUFFER_SIZE);
        moodycamel::ReaderWriterQueue<float> ringBufferRight = moodycamel::ReaderWriterQueue<float>(BUFFER_SIZE);
        std::vector<float> bufferLeft = std::vector<float>(BUFFER_SIZE);
        std::vector<float> bufferRight = std::vector<float>(BUFFER_SIZE);

        // For now we assume 16 bit WAV destination format, so 2 bytes per sample
        std::vector<char> byteBufferLeft = std::vector<char>(BUFFER_SIZE * 2);
        std::vector<char> byteBufferRight = std::vector<char>(BUFFER_SIZE * 2);
        std::vector<char> stereoByteBuffer = std::vector<char>(BUFFER_SIZE * 2 * 2);

        static const std::vector<std::pair<std::string, std::string>> fileNamesMono;
        static const std::vector<std::string> fileNamesStereo;

        int index = 0;
		mpc::engine::audio::core::AudioFormat* outputFileFormat = nullptr;
		std::vector<std::ofstream> fileStreams;
		std::atomic<bool> writing = ATOMIC_VAR_INIT(false);
		std::atomic<bool> preparedToWrite = ATOMIC_VAR_INIT(false);
		int writtenByteCount = 0;
		int lengthInFrames = 0;
		int lengthInBytes = 0;

        void writeRingBufferToDisk();

	public:
		bool start();
		bool stopEarly();
		bool prepare(int lengthInFrames, int sampleRate, bool isStereo);
		int processAudio(mpc::engine::audio::core::AudioBuffer* buf, int nFrames) override;

	public:
		DiskRecorder(mpc::Mpc&, mpc::engine::audio::core::AudioProcess* process, int index);
        ~DiskRecorder();

	};
}

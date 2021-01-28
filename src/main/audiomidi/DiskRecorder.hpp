#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <fstream>
#include <string>

#ifdef __linux__
#include <atomic>
#endif

namespace mpc::audiomidi {

	class DiskRecorder
		: public ctoot::audio::core::AudioProcessAdapter

	{

	private:
		std::string name;
		ctoot::audio::core::AudioFormat* format = nullptr;
		std::ofstream fileStream;
		std::atomic<bool> writing = ATOMIC_VAR_INIT(false);
		int writtenByteCount = 0;
		int lengthInFrames = 0;
		int lengthInBytes = 0;
		int sampleRate = 0;

	public:
		bool start();
		bool stopEarly();
		bool prepare(const std::string& absolutePath, int lengthInFrames, int sampleRate);
		int processAudio(ctoot::audio::core::AudioBuffer* buf, int nFrames) override;

	public:
		DiskRecorder(ctoot::audio::core::AudioProcess* process, std::string name);
		~DiskRecorder();

	};
}

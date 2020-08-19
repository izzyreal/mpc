#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <fstream>
#include <string>

namespace mpc::audiomidi {

	class DiskRecorder
		: public ctoot::audio::core::AudioProcessAdapter

	{

	private:
		std::string name;
		ctoot::audio::core::AudioFormat* format = nullptr;
		std::ofstream fileStream;
		bool writing = false;
		int written = 0;
		int lengthInFrames = 0;
		int lengthInBytes = 0;
		int sampleRate = 0;

	public:
		void start();
		void stop();
		void prepare(const std::string& absolutePath, int lengthInFrames, int sampleRate);
		int processAudio(ctoot::audio::core::AudioBuffer* buf) override;

	public:
		DiskRecorder(ctoot::audio::core::AudioProcess* process, std::string name);
		~DiskRecorder();

	};
}

#pragma once

#include "../mpc_fs.hpp"

#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioProcess.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <utility>

#ifdef __linux__
#include <atomic>
#endif

namespace mpc { class Mpc; }

namespace mpc::audiomidi {

	class DiskRecorder
		: public mpc::engine::audio::core::AudioProcessAdapter

	{

	private:
        static const std::vector<std::pair<std::string, std::string>> fileNamesMono;
        static const std::vector<std::string> fileNamesStereo;

        int index = 0;
		mpc::engine::audio::core::AudioFormat* format = nullptr;
		std::vector<std::ofstream> fileStreams;
		std::atomic<bool> writing = ATOMIC_VAR_INIT(false);
		int writtenByteCount = 0;
		int lengthInFrames = 0;
		int lengthInBytes = 0;

	public:
		bool start();
		bool stopEarly();
		bool prepare(mpc::Mpc&, int lengthInFrames, int sampleRate, bool isStereo);
		int processAudio(mpc::engine::audio::core::AudioBuffer* buf, int nFrames) override;

	public:
		DiskRecorder(mpc::engine::audio::core::AudioProcess* process, int index);
        ~DiskRecorder();

	};
}

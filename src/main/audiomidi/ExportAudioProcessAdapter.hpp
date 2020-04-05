#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <fstream>
#include <string>

namespace mpc {
	namespace audiomidi {

		class ExportAudioProcessAdapter
			: public ctoot::audio::core::AudioProcessAdapter

		{

		private:
			std::string name;
			std::shared_ptr<ctoot::audio::core::AudioFormat> format;
			std::ofstream fileStream;
			bool writing = false;
			int written = 0;
			int lengthInFrames = 0;
			int lengthInBytes = 0;
			int sampleRate = 0;

		public:
			void start();
			void prepare(const std::string& absolutePath, int lengthInFrames, int sampleRate);
			int processAudio(ctoot::audio::core::AudioBuffer* buf) override;

		public:
			ExportAudioProcessAdapter(ctoot::audio::core::AudioProcess* process, std::shared_ptr<ctoot::audio::core::AudioFormat> format, std::string name);
			~ExportAudioProcessAdapter();

		};

	}
}

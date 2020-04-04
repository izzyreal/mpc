#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <fstream>

namespace moduru {
	namespace io {
		class FileOutputStream;
	}
	namespace file {
		class File;
	}
}

namespace mpc {
	namespace audiomidi {

		class ExportAudioProcessAdapter
			: public ctoot::audio::core::AudioProcessAdapter

		{

		private:
			std::string name;
			std::shared_ptr<ctoot::audio::core::AudioFormat> format;
			bool writing = false;
			int written = 0;
			moduru::file::File* file = nullptr;
			std::fstream tempFileRaf;
			int lengthInBytes;
			int sampleRate;

		public:
			void start();
			void prepare(moduru::file::File* file, int lengthInFrames, int sampleRate);
			int processAudio(ctoot::audio::core::AudioBuffer* buf) override;
			void writeWav();

		public:
			ExportAudioProcessAdapter(ctoot::audio::core::AudioProcess* process, std::shared_ptr<ctoot::audio::core::AudioFormat> format, std::string name);
			~ExportAudioProcessAdapter();

		};

	}
}

#pragma once
#include <audio/core/AudioProcessAdapter.hpp>
#include <audio/core/AudioProcess.hpp>

#include <fstream>
#include <thread>

//#include <io/CircularBuffer.hpp>

//#include <boost/circular_buffer.hpp>

#include <io/TSCircularBuffer.hpp>

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
			typedef ctoot::audio::core::AudioProcessAdapter super;

		private:
			std::string name{ "" };
			//std::unique_ptr<moduru::io::CircularBuffer> circularBuffer{};
			circular_buffer<char>* circularBuffer = new circular_buffer<char>(2000000);
			std::weak_ptr<ctoot::audio::core::AudioFormat> format{ };
			bool reading{ false };
			bool writing{ false };
			moduru::file::File* file{ nullptr };

		private:
			//moduru::io::FileOutputStream* tempFileFos{ nullptr };
			std::fstream tempFileRaf{};
			std::thread writeThread;
			int lengthInBytes{};
			int sampleRate;

		private:
			static void static_startWriting(void * args);
			void startWriting();

		public:
			void prepare(moduru::file::File* file, int lengthInFrames, int sampleRate);
			void start();
			int processAudio(ctoot::audio::core::AudioBuffer* buf) override;
			virtual void stop();
			void writeWav();

		public:
			ExportAudioProcessAdapter(ctoot::audio::core::AudioProcess* process, std::weak_ptr<ctoot::audio::core::AudioFormat> format, std::string name);
			~ExportAudioProcessAdapter();

		};

	}
}

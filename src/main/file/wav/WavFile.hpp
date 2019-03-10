#pragma once

#include <vector>
#include <memory>

#include <io/CachedOutputStream.hpp>
#include <file/File.hpp>
#include <io/FileInputStream.hpp>
#include <io/BufferedOutputStream.hpp>

namespace mpc {
	namespace file {
		namespace wav {

			class WavFile
			{

			private:
				static const int BUFFER_SIZE{ 2048 };
				static const int FMT_CHUNK_ID{ 544501094 };
				static const int DATA_CHUNK_ID{ 1635017060 };
				static const int RIFF_CHUNK_ID{ 1179011410 };
				static const int RIFF_TYPE_ID{ 1163280727 };
				//WavFile_IOState* ioState{};
				int bytesPerSample{};
				int numFrames{};
				std::unique_ptr<moduru::io::BufferedOutputStream> oStream{};
				std::unique_ptr<moduru::io::CachedOutputStream> cache{};
				std::unique_ptr<moduru::io::FileInputStream> iStream{};
				double floatScale{};
				double floatOffset{};
				bool wordAlignAdjust{};
				int numChannels{};
				int sampleRate{};
				int blockAlign{};
				int validBits{};
				std::vector<char> buffer;
				int bufferPointer{};
				int bytesRead{};
				int frameCounter{};

			public:
				int getNumChannels();
				int getNumFrames();
				int getFramesRemaining();
				int getSampleRate();
				int getValidBits();
				WavFile* newWavFile(int numChannels, int numFrames, int validBits, int sampleRate) ;
				void openWavFile(std::weak_ptr<moduru::file::File> file) ;

			private:
				int getLE(int pos, int numBytes);
				void putLE(int val, int pos, int numBytes);
				void writeSample(int val) ;
				int readSample();

			public:
				int readFrames(std::vector<int>* sampleBuffer, int numFramesToRead) ;
				int readFrames(std::vector<int>* sampleBuffer, int offset, int numFramesToRead) ;
				int readFrames(std::vector<std::vector<int>>* sampleBuffer, int numFramesToRead) ;
				int readFrames(std::vector<std::vector<int>>* sampleBuffer, int offset, int numFramesToRead) ;
				int writeFrames(std::vector<int>* sampleBuffer, int numFramesToWrite) ;
				int writeFrames(std::vector<int>* sampleBuffer, int offset, int numFramesToWrite) ;
				int writeFrames(std::vector<std::vector<int>>* sampleBuffer, int numFramesToWrite) ;
				int writeFrames(std::vector<std::vector<int>>* sampleBuffer, int offset, int numFramesToWrite) ;
				int readFrames(std::vector<float>* sampleBuffer, int numFramesToRead) ;
				int readFrames(std::vector<float>* sampleBuffer, int offset, int numFramesToRead) ;
				int readFrames(std::vector<std::vector<float>>* sampleBuffer, int numFramesToRead) ;
				int readFrames(std::vector<std::vector<float>>* sampleBuffer, int offset, int numFramesToRead) ;
				int writeFrames(std::vector<float>* sampleBuffer, int numFramesToWrite) ;
				int writeFrames(std::vector<float>* sampleBuffer, int offset, int numFramesToWrite) ;
				int writeFrames(std::vector<std::vector<float>>* sampleBuffer, int numFramesToWrite) ;
				int writeFrames(std::vector<std::vector<float>>* sampleBuffer, int offset, int numFramesToWrite) ;
				void close();
				std::vector<char> getResult();

			public:
				WavFile();

			private:
				//friend class WavFile_IOState;
			};
		}
	}
}

#pragma once
#include <VecUtil.hpp>
#include <file/ByteUtil.hpp>

#include <string>
#include <vector>

namespace mpc {

	namespace disk {
		class MpcFile;
	}

	namespace file {
		namespace sndreader {

			class SndHeaderReader;

			class SndReader
			{

			private:
				std::vector<char> sndFileArray{};
				SndHeaderReader* sndHeader{};
				mpc::disk::MpcFile* sndFile{};

			public:
				std::string getName();
				bool isMono();
				int getNumberOfFrames();
				int getSampleRate();
				int getLevel();
				int getStart();
				int getEnd();
				int getLoopLength();
				bool isLoopEnabled();
				int getTune();
				int getNumberOfBeats();
				void getSampleData(std::vector<float>* dest);

			public:
				std::vector<char> getSndFileArray();

			public:
				SndReader(mpc::disk::MpcFile* soundFile);

			};

		}
	}
}

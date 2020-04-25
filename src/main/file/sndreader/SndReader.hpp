#pragma once
#include <VecUtil.hpp>
#include <file/ByteUtil.hpp>

#include <string>
#include <vector>
#include <memory>

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::file::sndreader {
	class SndHeaderReader;
}

using namespace std;

namespace mpc::file::sndreader {

	class SndReader
	{

	private:
		vector<char> sndFileArray;
		shared_ptr<SndHeaderReader> sndHeaderReader;
		mpc::disk::MpcFile* sndFile;

	public:
		string getName();
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
		void getSampleData(vector<float>* dest);

	public:
		vector<char>& getSndFileArray();

	public:
		SndReader(mpc::disk::MpcFile* soundFile);

	};
}

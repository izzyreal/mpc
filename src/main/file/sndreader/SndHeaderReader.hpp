#pragma once

#include <string>
#include <vector>

namespace mpc::file::sndreader {
	class SndReader;
}

using namespace std;

namespace mpc::file::sndreader {

	class SndHeaderReader
	{

	private:
		vector<char> headerArray = vector<char>(42);

	public:
		vector<char>& getHeaderArray();
		bool hasValidId();
		string getName();
		int getLevel();
		int getTune();
		bool isMono();
		int getStart();
		int getEnd();
		int getNumberOfFrames();
		int getLoopLength();
		bool isLoopEnabled();
		int getNumberOfBeats();
		int getSampleRate();

	public:
		SndHeaderReader(SndReader* sndReader);

	};
}

#pragma once

#include <vector>
#include <string>

namespace mpc::file::sndwriter
{
	class SndWriter;
}

namespace mpc::file::sndwriter
{
	class SndHeaderWriter
	{
	private:
		std::vector<char> headerArray = std::vector<char>(42);

	public:
		std::vector<char>& getHeaderArray();
		void setFirstTwoBytes();
		void setName(const std::string& name);
		void setLevel(int i);
		void setTune(int i);
		void setMono(bool b);
		void setStart(int i);
		void setEnd(int i);
		void setFrameCount(int i);
		void setLoopLength(int i);
		void setLoopEnabled(bool b);
		void setBeatCount(int i);
		void setSampleRate(int i);

	private:
		void putLE(int offset, int value);

	public:
		SndHeaderWriter();
	};
}

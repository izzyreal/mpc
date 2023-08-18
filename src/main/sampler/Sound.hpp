#pragma once
#include <Observer.hpp>

namespace mpc::sampler {
	class Sound : public Observable
	{

	private:
		std::string name;
		int numberOfBeats{ 4 };
        bool mono = false, loopEnabled = false;
        int sampleRate = 44100;
        int sndLevel = 100, tune = 0;
        int start = 0, end = 0, loopTo = 0;
        std::vector<float> sampleData;

	public:
		void setName(std::string s);
		std::vector<float>* getSampleData();
		int getTune();
		int getStart();
		int getEnd();
		bool isLoopEnabled();
		int getLoopTo();
		bool isMono();
		int getLastFrameIndex();
		int getSndLevel();

		void setBeatCount(int i);
		int getBeatCount();
		std::string getName();
		int getSampleRate();
		void setSampleRate(int sr);
		int getFrameCount();
		void setMono(bool mono);
		void setEnd(int end);
		void setLevel(int level);
		void setStart(int start);
		void setLoopEnabled(bool loopEnabled);
		void setLoopTo(int loopTo);
		void setTune(int tune);
		void insertFrame(std::vector<float> frame, unsigned int index);
        void insertFrames(std::vector<float>& frames, unsigned int index, uint32_t nFrames = 0);
        void insertFrames(std::vector<float>& left, std::vector<float>& right, unsigned int index, uint32_t nFrames = 0);
        void appendFrames(std::vector<float>& frames, uint32_t nFrames = 0);
        void appendFrames(std::vector<float>& left, std::vector<float>& right, uint32_t nFrames = 0);
        void removeFramesFromEnd(int numFramesToRemove);

	public:
		Sound(int rate);

	};
}

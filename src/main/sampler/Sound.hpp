#pragma once
#include <engine/mpc/MpcSound.hpp>
#include <observer/Observable.hpp>

namespace mpc::sampler {
	class Sound
		: public virtual ctoot::mpc::MpcSound
		, public moduru::observer::Observable
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
		int getTune() override;
		int getStart() override;
		int getEnd() override;
		bool isLoopEnabled() override;
		int getLoopTo() override;
		bool isMono() override;
		int getLastFrameIndex() override;
		int getSndLevel() override;

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
        void insertFrames(std::vector<float>& frames, unsigned int index);
        void insertFrames(std::vector<float>& left, std::vector<float>& right, unsigned int index);

	public:
		Sound(int rate);

	};
}

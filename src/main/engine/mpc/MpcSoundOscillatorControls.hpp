#pragma once
#include <engine/control/CompoundControl.hpp>

#include <vector>
#include <string>

namespace ctoot::mpc
{
	class MpcSoundOscillatorControls
		:
		public ctoot::control::CompoundControl
	{

	private:
		bool mono = false, loopEnabled = false;
		int sampleRate = 0;
		int sndLevel = 0, tune = 0;
		int start = 0, end = 0, loopTo = 0;
		std::vector<float> sampleData;

	public:
		void setSndLevel(int i);
		void setTune(int i);
		void setLoopEnabled(bool b);
		void setStart(int i);
		void setEnd(int i);
		void setMono(bool b);
		void setLoopTo(int i);
		int getLastFrameIndex();
		int getFrameCount();
		int getTune();
		bool isLoopEnabled();
		int getStart();
		int getEnd();
		std::vector<float>* getSampleData();
		bool isMono();
		int getLoopTo();
		int getSampleRate();
		void setSampleRate(int sr);
		int getSndLevel();
		void insertFrame(std::vector<float> frame, unsigned int index);
		void insertFrames(std::vector<float>& frames, unsigned int index);
		void insertFrames(std::vector<float>& left, std::vector<float>& right, unsigned int index);

	public:
		MpcSoundOscillatorControls(int id);
		~MpcSoundOscillatorControls();

	public:
		std::string getName();

	};
}

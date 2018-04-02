#pragma once
#include <observer/Observable.hpp>
#include <ctootextensions/MpcSoundOscillatorControls.hpp>

namespace mpc {
	namespace sampler {

		class Sound
			: public moduru::observer::Observable
		{

		private:
			std::string name{ "" };
			int memoryIndex{ -1 };
			int numberOfBeats{ 4 };
			ctootextensions::MpcSoundOscillatorControls* msoc{ nullptr };

		public:
			void setName(std::string s);

		public:
			int getMemoryIndex();

		public:
			void setMemoryIndex(int i);
			void setNumberOfBeats(int i);
			int getBeatCount();
			std::string getName();
			std::vector<float>* getSampleData();
			int getSndLevel();
			int getTune();
			int getStart();
			int getEnd();
			bool isLoopEnabled();
			int getLoopTo();
			bool isMono();
			int getSampleRate();
			void setSampleRate(int sr);
			int getLastFrameIndex();
			void setMono(bool mono);
			void setEnd(int end);
			void setLevel(int level);
			void setStart(int start);
			void setLoopEnabled(bool loopEnabled);
			void setLoopTo(int loopTo);
			void setTune(int tune);
			ctootextensions::MpcSoundOscillatorControls* getMsoc();
			void insertFrame(std::vector<float> frame, unsigned int index);

		public:
			Sound(int rate, int index);
			Sound();
			~Sound();

		};

	}
}

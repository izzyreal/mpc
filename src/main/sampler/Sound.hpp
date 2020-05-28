#pragma once
#include <mpc/MpcSound.hpp>
#include <observer/Observable.hpp>
#include <mpc/MpcSoundOscillatorControls.hpp>

namespace mpc {
	namespace sampler {

		class Sound
			: public virtual ctoot::mpc::MpcSound
			, public moduru::observer::Observable
		{

		private:
			std::string name = "";
			int memoryIndex{ -1 };
			int numberOfBeats{ 4 };
			ctoot::mpc::MpcSoundOscillatorControls* oscillatorControls{ nullptr };

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
			int getFrameCount();
			void setMono(bool mono);
			void setEnd(int end);
			void setLevel(int level);
			void setStart(int start);
			void setLoopEnabled(bool loopEnabled);
			void setLoopTo(int loopTo);
			void setTune(int tune);
			ctoot::mpc::MpcSoundOscillatorControls* getOscillatorControls();
			void insertFrame(std::vector<float> frame, unsigned int index);

		public:
			Sound(int rate, int index);
			Sound();
			~Sound();

		};

	}
}

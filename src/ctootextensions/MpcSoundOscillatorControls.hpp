#pragma once
#include <control/CompoundControl.hpp>

namespace mpc {
	namespace ctootextensions {

		class MpcSoundOscillatorControls
			:
			public ctoot::control::CompoundControl
		{

		private:
			bool mono{false}, loopEnabled{false};
			int sampleRate{ 0 };
			int sndLevel{ 0 }, tune{ 0 };
			int start{ 0 }, end{ 0 }, loopTo{ 0 };
			std::vector<float> sampleData{};

		public:
			void setSndLevel(int i);
			void setTune(int i);
			void setLoopEnabled(bool b);
			void setStart(int i);
			void setEnd(int i);
			void setMono(bool b);
			void setLoopTo(int i);
			int getLastFrameIndex();
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

		public:
			MpcSoundOscillatorControls(int id, int instanceIndex);
			~MpcSoundOscillatorControls();

		public:
			std::string getName();

		};
	}
}

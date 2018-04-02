#pragma once
#include <observer/Observable.hpp>

#include <string>

namespace mpc {
	namespace sequencer {

		class Sequencer;
		class Step;

		class Song
			: public moduru::observer::Observable
		{

		private:
			std::string name{ "" };
			std::vector<Step*> steps{};
			bool loopEnabled{ false };
			int firstStep{ 0 };
			int lastStep{ 0 };
			Sequencer* sequencer{ nullptr };

		public:
			bool used{ false };

		public:
			void setLoopEnabled(bool b);
			bool isLoopEnabled();
			void setFirstStep(int i);
			int getFirstStep();
			void setLastStep(int i);
			int getLastStep();
			void setName(std::string string);
			std::string getName();
			void deleteStep(int stepIndex);
			void insertStep(int stepIndex, Step* s);

		private:
			void notifySong();

		public:
			void setStep(int i, Step* s);
			Step* getStep(int i);
			int getStepAmount();
			int getStepBarAmount(int step);
			bool isUsed();
			void setUsed(bool b);

			Song(Sequencer* sequencer);
			~Song();

		private:
			friend class Step;
		};

	}
}

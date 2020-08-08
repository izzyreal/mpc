#pragma once

#include <vector>
#include <memory>
#include <string>

namespace mpc::sequencer {
	class Step;
}

namespace mpc::sequencer
{
	class Song
	{

	private:
		std::string name = "";
		std::vector<std::shared_ptr<Step>> steps;
		bool loopEnabled = false;
		int firstStep = 0;
		int lastStep = 0;
		bool used = false;

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
		void insertStep(int stepIndex);

	public:
		std::weak_ptr<Step> getStep(int i);
		int getStepCount();
		bool isUsed();
		void setUsed(bool b);

	};
}

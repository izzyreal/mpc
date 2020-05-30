#pragma once
#include <observer/Observable.hpp>

#include <vector>
#include <string>

namespace mpc::ui::sampler::window {

	class SamplerWindowGui
		: public moduru::observer::Observable
	{

	private:
		int deletePgm{ 0 };
		int pgm0{ 0 };
		int pgm1{ 0 };
		int newProgramChange{ 1 };

		bool initPadAssignMaster{ false };

	public:
		int getDeletePgm();
		void setDeletePgm(int i, int programCount);
		int getPgm0();
		void setPgm0(int i, int programCount);
		int getPgm1();
		void setPgm1(int i, int programCount);
		int getNewProgramChange();
		void setNewProgramChange(int i);
		void setNewName(std::string s) {}
		std::string getNewName() { return ""; }

	public:
		SamplerWindowGui();
		~SamplerWindowGui();

	};
}

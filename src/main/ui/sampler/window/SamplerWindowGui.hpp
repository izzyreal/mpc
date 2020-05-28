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
		int prog0{ 0 };
		int prog1{ 0 };
		int note0{ 0 };
		int note1{ 0 };
		int autoChromAssSnd{ 0 };
		int tune{ 0 };
		int originalKey{ 60 };

		std::string newName = "";
		bool initPadAssignMaster{ false };

	public:
		static std::vector<std::string> padFocusNames;

		int getDeletePgm();
		void setDeletePgm(int i, int programCount);
		int getPgm0();
		void setPgm0(int i, int programCount);
		int getPgm1();
		void setPgm1(int i, int programCount);
		std::string getNewName();
		void setNewName(std::string s);
		int getNewProgramChange();
		void setNewProgramChange(int i);
		int getPadNumberFromFocus(const std::string& focus, int bank);
		std::string getFocusFromPadNumber(const int& padNumber);
		int getProg0();
		void setProg0(int i, const int& programCount);
		int getProg1();
		void setProg1(int i, const int& programCount);
		int getNote0();
		void setNote0(int i);
		int getNote1();
		void setNote1(int i);
		int getAutoChromAssSnd();
		void setAutoChromAssSnd(int i, const int& soundCount);
		int getTune();
		void setTune(int i);
		int getOriginalKey();
		void setOriginalKey(int i);

	public:
		SamplerWindowGui();
		~SamplerWindowGui();

	};
}

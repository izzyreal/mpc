#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc::ui::sampler
{
	class SamplerGui
		: public moduru::observer::Observable
	{

	private:
		bool padAssignMaster = false;
		int bank = 0;
		int pad = 0;
		int note = 60;
		std::string prevScreenName = "";
		int prevNote = 60;
		int prevPad;
		bool padToIntSound = true;
		std::string newName = "";
		int selectedDrum = 0;

	public:
		void setPadAndNote(int pad, int note);
		int getNote();
		int getPad();
		void setPadAssignMaster(bool b);
		bool isPadAssignMaster();
		void setBank(int i);
		int getBank();
		void setSelectedDrum(int i);
		int getSelectedDrum();
		std::string getPrevScreenName();
		void setPrevScreenName(std::string s);
		int getPrevNote();
		int getPrevPad();
		bool isPadToIntSound();
		void setPadToIntSound(bool b);
		void setNewName(std::string newName);
		std::string getNewName();
		void notify(std::string str);

	};
}

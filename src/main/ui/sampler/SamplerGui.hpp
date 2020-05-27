#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc::ui::sampler
{
	class SamplerGui
		: public moduru::observer::Observable
	{

	private:
		int pad = 0;
		int note = 60;
		std::string prevScreenName = "";
		int prevNote = 60;
		int prevPad;
		std::string newName = "";

	public:
		void setPadAndNote(int pad, int note);
		int getNote();
		int getPad();
		std::string getPrevScreenName();
		void setPrevScreenName(std::string s);
		int getPrevNote();
		int getPrevPad();
		void setNewName(std::string newName);
		std::string getNewName();
		void notify(std::string str);

	};
}

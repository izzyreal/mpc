#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc {
	
	namespace ui {

		namespace sampler {

			class SamplerGui
				: public moduru::observer::Observable
			{
				
			private:
				

			private:
				bool padAssignMaster{ false };
				int bank{ 0 };
				int pad{ 0 };
				int note{ 60 };
				std::string prevScreenName{ "" };
				int prevNote{ 60 };
				int prevPad{};
				bool padToIntSound{ true };
				int input{ 0 };
				int threshold{ -20 };
				int mode{ 1 };
				int time{ 100 };
				int monitor{ 0 };
				int preRec{ 100 };
				std::string newName{ "" };
				int selectedDrum{ 0 };

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
				int getInput();
				void setInput(int i);
				int getThreshold();
				void setThreshold(int i);
				int getMode();
				void setMode(int i);
				int getTime();
				void setTime(int i);
				int getMonitor();
				void setMonitor(int i);
				int getPreRec();
				void setPreRec(int i);
				void setNewName(std::string newName);
				std::string getNewName();
				void notify(std::string str);

			public:
				SamplerGui();
				~SamplerGui();

			};

		}
	}
}

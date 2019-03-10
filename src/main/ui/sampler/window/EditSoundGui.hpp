#pragma once
#include <observer/Observable.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	namespace ui {

		namespace sampler {
			namespace window {

				class EditSoundGui
					: public moduru::observer::Observable
				{

				private:
					int edit{ 0 };
					//int edit{ 7 };
					int insertSoundNumber{ 0 };
					int timeStretchRatio{ 10000 };
					//int timeStretchRatio{ 20000 };
					//int timeStretchRatio{ 20000 };
					int timeStretchPresetNumber{ 0 };
					int timeStretchAdjust{ 0 };
					int endMargin{ 30 };

					bool createNewProgram{ false };
					
					std::string newName{""};
					std::string previousScreenName{""};
					
					std::vector<std::string> newNames{};

				public:
					void setNewName(std::string s);
					std::string getNewName();
					void setEdit(int i);
					int getEdit();
					int getInsertSndNr();
					void setInsertSndNr(int i, int soundCount);
					void setTimeStretchRatio(int i);
					int getTimeStretchRatio();
					void setTimeStretchPresetNumber(int i);
					int getTimeStretchPresetNumber();
					void setTimeStretchAdjust(int i);
					int getTimeStretchAdjust();
					void setPreviousScreenName(std::string s);
					std::string getPreviousScreenName();
					void setNewName(int i, std::string s);
					std::string getNewName(int i);
					bool getCreateNewProgram();
					void setCreateNewProgram(bool b);
					int getEndMargin();
					void setEndMargin(int i);

				public:
					EditSoundGui();
					~EditSoundGui();

				};

			}
		}
	}
}

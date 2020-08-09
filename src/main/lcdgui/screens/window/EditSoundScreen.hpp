#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class EditSoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		EditSoundScreen(mpc::Mpc& mpc, const int layerIndex);
		void turnWheel(int j) override;
		void function(int j) override;
		void open() override;

		void setNewName(std::string s);

		void setReturnToScreenName(std::string s);
		std::string getReturnToScreenName();

	private:
		const std::vector<std::string> editNames{ "DISCARD", "LOOP FROM ST TO END", u8"SECTION \u00C4 NEW SOUND", u8"INSERT SOUND \u00C4 SECTION START", "DELETE SECTION", "SILENCE SECTION", "REVERSE SECTION", "TIME STRETCH", "SLICE SOUND" };
		std::vector<std::string> timeStretchPresetNames{ "FEM VOX", "MALE VOX", "LOW MALE VOX", "VOCAL", "HFREQ RHYTHM", "MFREQ RHYTHM", "LFREQ RHYTHM", "PERCUSSION", "LFREQ PERC.", "STACCATO", "LFREQ SLOW", "MUSIC 1", "MUSIC 2", "MUSIC 3", "SOFT PERC.", "HFREQ ORCH.", "LFREQ ORCH.", "SLOW ORCH." };

		void displayEdit();
		void displayCreateNewProgram();
		void displayEndMargin();
		void displayVariable();

	private:
		int edit = 0;
		int insertSoundNumber = 0;
		int timeStretchRatio = 10000;
		int timeStretchPresetNumber = 0;
		int timeStretchAdjust = 0;
		int endMargin = 30;
		bool createNewProgram = true;
		std::string newName = "";
		std::string returnToScreenName = "";

		void setEdit(int i);
		void setInsertSndNr(int i, int soundCount);
		void setTimeStretchRatio(int i);
		void setTimeStretchPresetNumber(int i);
		void setTimeStretchAdjust(int i);
		void setCreateNewProgram(bool b);
		void setEndMargin(int i);

	};
}

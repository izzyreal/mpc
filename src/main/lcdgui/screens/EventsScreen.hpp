#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens
{
	class BarsScreen;
	class TrMoveScreen;
}

namespace mpc::lcdgui::screens
{
	class EventsScreen
		: public mpc::lcdgui::ScreenComponent, public WithTimesAndNotes
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

		EventsScreen(const int layerIndex);

		void open() override;

	private:
		void displayStart();
		void displayTime();
		void displayCopies();
		void displayMode();
		void displayEdit();
		void displayNotes();
		void displayMidiNotes();
		void displayDrumNotes();
		void displayToTr();
		void displayToSq();
		void displayFromTr();
		void displayFromSq();
		void displayDurationMode();
		void displayVelocityMode();
		void displayTransposeAmount();
		void displayVelocityValue();

	private:
		std::vector<string> modeNames = { "ADD VALUE", "SUB VALUE", "MULTI VAL%", "SET TO VAL" };
		std::vector<string> functionNames = { "COPY", "DURATION", "VELOCITY", "TRANSPOSE" };
		bool modeMerge = false;
		int editFunctionNumber = 0;
		int drumNote = 34; // Should come from Mpc.getNote()?
		int fromTr = 0;
		int toSq = 0;
		int toTr = 0;
		int start = 0;
		int copies = 1;
		int durationMode = 0;
		int velocityMode = 0;
		int transposeAmount = 0;
		int durationValue = 1;
		int velocityValue = 1;

	public:
		void setEdit(int i);
		void setDrumNote(int i);
		void setFromSq(int i);
		void setFromTr(int i);
		void setToSq(int i);
		void setToTr(int i);
		void setModeMerge(bool b);
		void setCopies(int i);
		void setDurationMode(int i);
		void setVelocityMode(int i);
		void setTransposeAmount(int i);
		void setDuration(int i);
		void setVelocityValue(int i);
		void setStart(int startTicks);

	private:
		friend class mpc::lcdgui::screens::BarsScreen;
		friend class mpc::lcdgui::screens::TrMoveScreen;

	};
}

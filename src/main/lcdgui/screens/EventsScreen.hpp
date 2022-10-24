#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens
{
	class BarsScreen;
	class TrMoveScreen;
	class UserScreen;
}

namespace mpc::lcdgui::screens
{
	class EventsScreen
		: public mpc::lcdgui::ScreenComponent, public WithTimesAndNotes
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

		EventsScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
        void pad(int padIndexWithBank, int velo) override;

        void performCopy(int sourceStart, int sourceEnd, int toSequenceIndex,
                         int destStart, int toTrackIndex, bool copyModeMerge, int copyCount, int copyNote0, int copyNote1);

	private:
        void displayTime() override;
        void displayNotes() override;
        void displayDrumNotes() override;

        void displayStart();
		void displayCopies();
		void displayMode();
		void displayEdit();
		void displayMidiNotes();
		void displayToTr();
		void displayToSq();
		void displayFromTr();
		void displayFromSq();

	private:
		bool setNote1X = true;
		int tab = 0;
		const std::vector<std::string> tabNames{ "events", "bars", "tr-move", "user" };
		const std::vector<std::string> modeNames{ "ADD VALUE", "SUB VALUE", "MULTI VAL%", "SET TO VAL" };
		const std::vector<std::string> functionNames{ "COPY", "DURATION", "VELOCITY", "TRANSPOSE" };
		bool modeMerge = false;
		int editFunctionNumber = 0;
		int toSq = 0;
		int toTr = 0;
		int start = 0;
		int copies = 1;
		int durationMode = 0;
		int velocityMode = 0;
		int transposeAmount = 0;
		int durationValue = 1;
		int velocityValue = 1;

	private:
		void setEdit(int i);
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
		friend class mpc::lcdgui::screens::UserScreen;

	};
}

#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::audiomidi
{
	class EventHandler;
}

namespace mpc::lcdgui::screens::window
{
	class NameScreen;
}

namespace mpc::lcdgui::screens::window
{
	class VmpcDirectToDiskRecorderScreen
		: public mpc::lcdgui::ScreenComponent, private mpc::lcdgui::screens::WithTimesAndNotes
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		VmpcDirectToDiskRecorderScreen(const int layerIndex);

		void open() override;

	public:
		bool isOffline();
		int getSampleRate();

	private:
		const std::vector<std::string> recordNames{ "SEQUENCE", "LOOP", "CUSTOM RANGE", "SONG", "JAM" };
		int record = 0;
		int sq = 0;
		int song = 0;
		std::string outputFolder = "DEFAULT";
		bool offline = false;
		bool splitLR = true;
		int sampleRate = 0;

		void setRecord(int i);
		void setSq(int i);
		void setSong(int i);
		void setOffline(bool b);
		void setSplitLR(bool b);
		void setSampleRate(int rate);
		void setOutputFolder(std::string s);

		void displayRate();
		void displaySong();
		void displayOffline();
		void displaySplitLR();
		void displayOutputFolder();
		void displayRecord();
		void displaySq();
		
		void displayTime() override;
		void displayNotes() override {} // We don't have notes in this screen

		friend class NameScreen;
		friend class EventHandler;

	};
}

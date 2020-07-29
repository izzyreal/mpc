#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::audiomidi
{
	class EventHandler;
	class MpcMidiInput;
}

namespace mpc::file::all
{
	class MidiSyncMisc;
	class Misc;
}

namespace mpc::disk
{
	class AllLoader;
}

namespace mpc::lcdgui::screens
{
	class SyncScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

	public:
		SyncScreen(const int layerIndex);

		void open() override;

		std::vector<int> modeIns{ 0, 0 };
		std::vector<int> modeOuts{ 0, 0 };

	private:
		const std::vector<std::string> modeNames{ "OFF", "MIDI CLOCK", "TIME CODE" };

		int shiftEarly = 0;
		bool sendMMCEnabled = false;
		int frameRate = 0;
		bool receiveMMCEnabled = false;
		int out = 0;
		int in = 0;

		void setIn(int i);
		void setOut(int i);
		void setShiftEarly(int i);
		void setSendMMCEnabled(bool b);
		void setFrameRate(int i);
		void setModeIn(int i);
		void setModeOut(int i);
		int getModeOut();
		void setReceiveMMCEnabled(bool b);

		void displayIn();
		void displayOut();
		void displayModeIn();
		void displayModeOut();
		void displayReceiveMMC();
		void displaySendMMC();
		void displayShiftEarly(); // Also used to display "Frame rate:" field

	public:
		int getModeIn();

	private:
		friend class mpc::audiomidi::EventHandler;
		friend class mpc::audiomidi::MpcMidiInput;
		friend class mpc::disk::AllLoader;
		friend class mpc::file::all::MidiSyncMisc;
		friend class mpc::file::all::Misc;

	};
}

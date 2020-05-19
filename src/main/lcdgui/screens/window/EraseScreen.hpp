#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens::window {

	class EraseScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::lcdgui::screens::WithTimesAndNotes
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

	public:
		EraseScreen(const int& layer);

	public:
		void open() override;
		void close() override;

	public:
		void update(moduru::observer::Observable* observable, nonstd::any message);

	private:
		void displayTrack();
		void displayErase();
		void displayType();

	protected:
		void displayNotes() override;
		void displayTime() override;

	private:
		std::vector<std::string> eventClassNames = vector<string>{ "com.mpc.sequencer.NoteEvent", "com.mpc.sequencer.PitchBendEvent", "com.mpc.sequencer.ControlChangeEvent", "com.mpc.sequencer.ProgramChangeEvent", "com.mpc.sequencer.ChannelPressureEvent", "com.mpc.sequencer.PolyPressureEvent", "com.mpc.sequencer.SystemExclusiveEvent" };
		std::vector<std::string> typeNames = vector<string>{ "NOTES", "PITCH BEND", "CONTROL", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE" };
		std::vector<std::string> eraseNames = vector<string>{ "ALL EVENTS", "ALL EXCEPT", "ONLY ERASE" };

		int track = 0;
		int erase = 0;
		int type = 0;
		int midiNote0 = 0;
		int midiNote1 = 127;
		int time0 = 0;
		int time1 = 0;

		void setTrack(int i);
		void setErase(int i);
		void setType(int i);

	};
}

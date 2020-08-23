#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens::window
{
	class EraseScreen
		: public mpc::lcdgui::ScreenComponent, public mpc::lcdgui::screens::WithTimesAndNotes
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

	public:
		EraseScreen(mpc::Mpc& mpc, const int layerIndex);

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
		const std::vector<std::string> eventClassNames{ "com.mpc.sequencer.NoteEvent", "com.mpc.sequencer.PitchBendEvent", "com.mpc.sequencer.ControlChangeEvent", "com.mpc.sequencer.ProgramChangeEvent", "com.mpc.sequencer.ChannelPressureEvent", "com.mpc.sequencer.PolyPressureEvent", "com.mpc.sequencer.SystemExclusiveEvent" };
		const std::vector<std::string> typeNames{ "NOTES", "PITCH BEND", "CONTROL", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE" };
		const std::vector<std::string> eraseNames{ "ALL EVENTS", "ALL EXCEPT", "ONLY ERASE" };

		int track = 0;
		int erase = 0;
		int type = 0;

		void setTrack(int i);
		void setErase(int i);
		void setType(int i);

	};
}

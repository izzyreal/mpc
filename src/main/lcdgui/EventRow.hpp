#pragma once

#include <lcdgui/Component.hpp>

#include <memory>
#include <vector>
#include <string>

namespace ctoot::mpc
{
	class MpcSoundPlayerChannel;
}

namespace mpc { class Mpc; }

namespace mpc::sequencer
{
		class Event;
}

namespace mpc::lcdgui
{
	class EventRowParameters;
	class Field;
	class Label;
	class HorizontalBar;
}

namespace mpc::lcdgui
{
	class EventRow
		: public Component
	{
	private:
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sequencer::Event> event;
		std::shared_ptr<mpc::lcdgui::HorizontalBar> horizontalBar;
		std::shared_ptr<mpc::lcdgui::EventRowParameters> parameters;
		std::vector<std::shared_ptr<mpc::lcdgui::Field>> fields;
		std::vector<std::shared_ptr<mpc::lcdgui::Label>> labels;

		const std::vector<std::string> letters{ "a", "b", "c", "d", "e" };
		const std::vector<std::string> noteVarParamNames{ "Tun", "Dcy", "Atk", "Flt" };
		const std::vector<std::string> drumNoteEventLabels{ ">N: ", "", ":", "D:", "V:" };
		const std::vector<int> drumNoteEventSizes{ 6, 3, 4, 4, 3 };
		const std::vector<int> drumNoteEventXPos{ 0, 66, 85, 120, 162 };
		const std::vector<std::string> midiNoteEventLabels{ ">Note:", "D:", "V:" };
		const std::vector<int> midiNoteEventSizes{ 8, 4, 3 };
		const std::vector<int> midiNoteEventXPos{ 0, 98, 144 };
		const std::vector<std::string> miscEventLabels{ ">BEND          :", ":" };
		const std::vector<int> miscEventSizes{ 5, 0 };
		const std::vector<int> miscEventXPos{ 0, 168 };
		const std::vector<std::string> polyPressureEventLabels{ ">POLY PRESSURE :", ":" };
		const std::vector<int> polyPressureEventSizes{ 8, 3 };
		const std::vector<int> polyPressureEventXPos{ 0, 168 };
		const std::vector<std::string> sysexEventLabels{ ">Exclusive:", "" };
		const std::vector<int> sysexEventSizes{ 2, 2 };
		const std::vector<int> sysexEventXPos{ 0, 81 };
		const std::vector<std::string> emptyEventLabels{ "" };
		const std::vector<int> emptyEventSizes{ 1 };
		const std::vector<int> emptyEventXPos{ 6 };
		const std::vector<std::string> channelPressureEventLabels{ ">CH PRESSURE   :            :" };
		const std::vector<int> channelPressureEventSizes{ 3 };
		const std::vector<int> channelPressureEventXPos{ 0 };
		std::vector<std::string> controlChangeEventLabels{ ">CONTROL CHANGE:", ":" };
		std::vector<int> controlChangeEventSizes{ 12, 3 };
		std::vector<int> controlChangeEventXPos{ 0, 168 };
		std::vector<std::string> mixerParamNames{ "STEREO LEVEL", "STEREO PAN  ", "FXsend LEVEL", "INDIV LEVEL" };
		std::vector<std::string> mixerEventLabels{ ">", "N:", "L:" };
		std::vector<int> mixerEventSizes{ 12, 6, 3 };
		std::vector<int> mixerEventXPos{ 0, 96, 162 };

		bool selected = false;
		int bus = -1;
		int rowIndex = 0;

	public:
		void setBus(const int newBus);
		void init();
		void setEmptyEventValues();
		void setSystemExclusiveEventValues();
		void setPolyPressureEventValues();
		void setChannelPressureEventValues();
		void setControlChangeEventValues();
		void setMiscEventValues();
		void setMixerEventValues();
		void setDrumNoteEventValues();
		void setMidiNoteEventValues();

	private:
		void setColors();
		void setLabelTexts(const std::vector<std::string>& labels);
		void setSizesAndLocations(const std::vector<int>& xPosArray, const std::vector<int>& sizeArray);

	public:
		void setEvent(std::weak_ptr<mpc::sequencer::Event> newEvent);
		void setSelected(bool b);
		bool isSelected();

		EventRow(mpc::Mpc& mpc, int rowNumber);

		static std::vector<std::string> controlNames;

	};
}

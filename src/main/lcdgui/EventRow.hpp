#pragma once

#include <lcdgui/Component.hpp>

#include <memory>
#include <vector>
#include <string>

namespace ctoot::mpc
{
	class MpcSoundPlayerChannel;
}

namespace mpc::sequencer
{
		class Event;
}

namespace mpc::sampler
{
	class Sampler;
	class Program;
}

namespace mpc::lcdgui
{
	class SelectedEventBar;
	class Field;
	class Label;
	class HorizontalBar;
}

namespace mpc::ui::sampler
{
	class SamplerGui;
}

namespace mpc::lcdgui
{

	class EventRow
	{

	private:
		std::vector<std::weak_ptr<mpc::lcdgui::Component>> eventRow;
		std::weak_ptr<mpc::sequencer::Event> event;
		std::weak_ptr<mpc::lcdgui::HorizontalBar> horizontalBar;
		std::weak_ptr<mpc::lcdgui::SelectedEventBar> selectedEventBar;
		std::vector<std::weak_ptr<mpc::lcdgui::Field>> tfArray;
		std::vector<std::weak_ptr<mpc::lcdgui::Label>> labelArray;
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel = nullptr;
		std::weak_ptr<mpc::sampler::Program> program;
		mpc::ui::sampler::SamplerGui* samplerGui = nullptr;

		std::vector<std::string> letters;
		std::vector<std::string> noteVarParamNames;
		std::vector<std::string> drumNoteEventLabels;
		std::vector<int> drumNoteEventSizes;
		std::vector<int> drumNoteEventXPos;
		std::vector<std::string> midiNoteEventLabels;
		std::vector<int> midiNoteEventSizes;
		std::vector<int> midiNoteEventXPos;
		std::vector<std::string> miscEventLabels;
		std::vector<int> miscEventSizes;
		std::vector<int> miscEventXPos;
		std::vector<std::string> polyPressureEventLabels;
		std::vector<int> polyPressureEventSizes;
		std::vector<int> polyPressureEventXPos;
		std::vector<std::string> sysexEventLabels;
		std::vector<int> sysexEventSizes;
		std::vector<int> sysexEventXPos;
		std::vector<std::string> emptyEventLabels;
		std::vector<int> emptyEventSizes;
		std::vector<int> emptyEventXPos;
		std::vector<std::string> channelPressureEventLabels;
		std::vector<int> channelPressureEventSizes;
		std::vector<int> channelPressureEventXPos;
		bool selected;
		std::vector<std::string> controlChangeEventLabels;
		std::vector<int> controlChangeEventSizes;
		std::vector<int> controlChangeEventXPos;
		std::vector<std::string> mixerParamNames;
		std::vector<std::string> mixerEventLabels;
		std::vector<int> mixerEventSizes;
		std::vector<int> mixerEventXPos;
		bool midi;
		int rowNumber;

	public:
		static std::vector<std::string> controlNames;
		void setMidi(bool b);
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
		void initLabelsAndFields();
		void setColors();
		void setLabelTexts(std::vector<std::string> labels);
		void setSizeAndLocation(std::vector<int> xPosArray, std::vector<int> sizeArray);

	public:
		std::vector<std::weak_ptr<mpc::lcdgui::Component>> getEventRow();
		void setEvent(std::weak_ptr<mpc::sequencer::Event> event);
		void setSelected(bool b);
		bool isSelected();

	public:
		EventRow(int bus, std::weak_ptr<mpc::sequencer::Event> e, int rowNumber);

	};
}

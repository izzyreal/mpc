#pragma once
#include <observer/Observer.hpp>

#include <sequencer/TimeSignature.hpp>

#include <memory>
#include <vector>

namespace ctoot::mpc
{
	class MpcSoundPlayerChannel;
}

namespace mpc::sequencer
{
	class Sequencer;
	class Sequence;
	class Track;
}

namespace mpc::sampler
{
	class Sampler;
	class Program;
}

namespace mpc::lcdgui
{
	class FunctionKeys;
	class Field;
	class Label;
}

namespace mpc::ui
{
	class NameGui;
}

namespace mpc::ui::sampler
{
	class SamplerGui;
}

namespace mpc::ui::sequencer::window
{
	class SequencerWindowGui;
}

namespace mpc::ui::sequencer::window
{

	class SequencerWindowObserver
		: public moduru::observer::Observer
	{

	private:

		std::vector<std::string> softThruNames;
		std::vector<std::string> inNames;
		std::vector<std::string> editTypeNames;
		std::vector<std::string> typeNames;
		std::vector<std::string> noteValueNames;
		std::string csn{ "" };
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		std::weak_ptr<mpc::sampler::Program> program;
		int trackNum = 0;
		int seqNum = 0;
		std::weak_ptr<mpc::sequencer::Sequence> sequence;
		std::weak_ptr<mpc::sequencer::Track> track;
		mpc::sequencer::TimeSignature* newTimeSignature;
		SequencerWindowGui* swGui = nullptr;
		mpc::ui::NameGui* nameGui = nullptr;
		std::weak_ptr<mpc::lcdgui::Field> sqField;
		std::weak_ptr<mpc::lcdgui::Field> trField;
		std::weak_ptr<mpc::lcdgui::Field> trackNameFirstLetterField;
		std::weak_ptr<mpc::lcdgui::Field> defaultTrackNameFirstLetterField;
		std::weak_ptr<mpc::lcdgui::Label> defaultTrackNameRestLabel;
		std::weak_ptr<mpc::lcdgui::Label> trackNameRestLabel;
		std::weak_ptr<mpc::lcdgui::Field> sq0Field;
		std::weak_ptr<mpc::lcdgui::Field> sq1Field;
		std::weak_ptr<mpc::lcdgui::Field> tr0Field;
		std::weak_ptr<mpc::lcdgui::Field> tr1Field;
		std::weak_ptr<mpc::lcdgui::Field> displayStyleField;
		std::weak_ptr<mpc::lcdgui::Field> startTimeField;
		std::weak_ptr<mpc::lcdgui::Field> hField;
		std::weak_ptr<mpc::lcdgui::Field> mField;
		std::weak_ptr<mpc::lcdgui::Field> sField;
		std::weak_ptr<mpc::lcdgui::Field> fField;
		std::weak_ptr<mpc::lcdgui::Field> frameRateField;
		std::vector<std::string> displayStyleNames;
		std::vector<std::string> frameRateNames;
		std::weak_ptr<mpc::lcdgui::Field> noteValueField;
		std::weak_ptr<mpc::lcdgui::Field> swingField;
		std::weak_ptr<mpc::lcdgui::Field> notes0Field;
		std::weak_ptr<mpc::lcdgui::Field> notes1Field;
		std::weak_ptr<mpc::lcdgui::Field> time0Field;
		std::weak_ptr<mpc::lcdgui::Field> time1Field;
		std::weak_ptr<mpc::lcdgui::Field> time2Field;
		std::weak_ptr<mpc::lcdgui::Field> time3Field;
		std::weak_ptr<mpc::lcdgui::Field> time4Field;
		std::weak_ptr<mpc::lcdgui::Field> time5Field;
		std::weak_ptr<mpc::lcdgui::Field> shiftTimingField;
		std::weak_ptr<mpc::lcdgui::Field> amountField;
		std::weak_ptr<mpc::lcdgui::Label> swingLabel;
		std::weak_ptr<mpc::lcdgui::Label> notes1Label;
		ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel;
		mpc::lcdgui::FunctionKeys* fb;
		std::weak_ptr<mpc::lcdgui::Field> bar0Field;
		std::weak_ptr<mpc::lcdgui::Field> bar1Field;
		std::weak_ptr<mpc::lcdgui::Field> newTsigField;
		std::weak_ptr<mpc::lcdgui::Field> numberOfBarsField;
		std::weak_ptr<mpc::lcdgui::Field> lastBarField;
		std::weak_ptr<mpc::lcdgui::Field> firstBarField;
		std::weak_ptr<mpc::lcdgui::Field> inThisTrackField;
		std::weak_ptr<mpc::lcdgui::Field> newBarsField;
		std::weak_ptr<mpc::lcdgui::Label> message0Label;
		std::weak_ptr<mpc::lcdgui::Label> message1Label;
		std::weak_ptr<mpc::lcdgui::Label> currentLabel;
		std::weak_ptr<mpc::lcdgui::Field> a0mrsField;
		std::weak_ptr<mpc::lcdgui::Field> a1mrsField;
		std::weak_ptr<mpc::lcdgui::Field> a2mrsField;
		std::weak_ptr<mpc::lcdgui::Field> b0mrsField;
		std::weak_ptr<mpc::lcdgui::Field> b1mrsField;
		std::weak_ptr<mpc::lcdgui::Field> b2mrsField;
		std::weak_ptr<mpc::lcdgui::Field> c0mrsField;
		std::weak_ptr<mpc::lcdgui::Field> c1mrsField;
		std::weak_ptr<mpc::lcdgui::Field> c2mrsField;
		std::weak_ptr<mpc::lcdgui::Field> passField;
		std::weak_ptr<mpc::lcdgui::Field> typeField;
		std::weak_ptr<mpc::lcdgui::Field> midiFilterField;
		std::weak_ptr<mpc::lcdgui::Field> sustainPedalToDurationField;
		std::weak_ptr<mpc::lcdgui::Field> progChangeSeqField;
		std::weak_ptr<mpc::lcdgui::Field> receiveChField;
		std::weak_ptr<mpc::lcdgui::Field> deviceNumberField;
		std::weak_ptr<mpc::lcdgui::Field> softThruField;
		std::weak_ptr<mpc::lcdgui::Field> editTypeField;
		std::weak_ptr<mpc::lcdgui::Field> valueField;
		std::weak_ptr<mpc::lcdgui::Label> deviceNameLabel;
		std::weak_ptr<mpc::lcdgui::Field> firstLetterField;
		mpc::ui::sampler::SamplerGui* samplerGui = nullptr;

	private:
		void displaySoftThru();
		void displayPass();
		void displayType();
		void displayMidiFilter();
		void displaySustainPedalToDuration();
		void displayProgChangeSeq();
		void displayReceiveCh();
		void displayMrsLine(int i);
		void displayNewBars();
		void displayTransmitProgramChangesInThisTrack();
		void displayNumberOfBars();
		void displayLastBar();
		void displayFirstBar();
		void displayBars();
		void displayNewTsig();
		void displayNoteValue();
		void displaySwing();
		void displayNotes();
		void displayTime();
		void displayShiftTiming();
		void displayAmount();
		void displayDisplayStyle();
		void displayStartTime();
		void displayFrameRate();

	public:
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	private:
		void displayDeviceName();
		void displayValue();
		void displayEditType();
		void displayTrackNumber();
		void displaySequenceNumberName();
		void displaySequenceNumberNames();
		void displayTrackNumberNames();

	public:
		SequencerWindowObserver();
		~SequencerWindowObserver();

	};
}

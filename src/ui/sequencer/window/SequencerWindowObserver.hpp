#pragma once
#include <observer/Observer.hpp>

#include <sequencer/TimeSignature.hpp>

#include <memory>
#include <vector>

namespace ctoot {
	namespace mpc {
		class MpcSoundPlayerChannel;
	}
}

namespace mpc {

	class Mpc;

	namespace sequencer {
		class Sequencer;
		class Sequence;
		class TempoChangeEvent;
		class Track;
	}
	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace lcdgui {
		class FunctionKeys;
		class Field;
		class Label;
		class HorizontalBar;
	}

	namespace ui {

		class NameGui;

		namespace sampler {
			class SamplerGui;
		}

		namespace sequencer {
			namespace window {

				class SequencerWindowGui;

				class SequencerWindowObserver
					: public moduru::observer::Observer
				{

				private:
					mpc::Mpc* mpc;
					std::vector<std::string> softThruNames{};
					std::vector<std::string> inNames{};
					std::vector<std::string> editTypeNames{};
					std::vector<std::string> typeNames{};
					std::vector<std::weak_ptr<mpc::sequencer::TempoChangeEvent>> visibleTempoChangeEvents{};
					std::vector<std::string> noteValueNames{};
					std::string csn{""};
					std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
					std::weak_ptr<mpc::sampler::Sampler> sampler{};
					std::weak_ptr<mpc::sampler::Program> program{};
					int trackNum{};
					int seqNum{};
					std::weak_ptr<mpc::sequencer::Sequence> sequence{};
					std::weak_ptr<mpc::sequencer::Track> track{};
					mpc::sequencer::TimeSignature timeSig;
					mpc::sequencer::TimeSignature* newTimeSignature;
					std::weak_ptr<mpc::lcdgui::Field> sequenceNameFirstLetterField{};
					std::weak_ptr<mpc::lcdgui::Field> defaultSequenceNameFirstLetterField{};
					std::weak_ptr<mpc::lcdgui::Label> sequenceNameRestLabel{};
					std::weak_ptr<mpc::lcdgui::Label> defaultSequenceNameRestLabel{};
					SequencerWindowGui* swGui{};
					mpc::ui::NameGui* nameGui{};
					std::weak_ptr<mpc::lcdgui::Field> sqField{};
					std::weak_ptr<mpc::lcdgui::Field> trField{};
					std::weak_ptr<mpc::lcdgui::Field> trackNameFirstLetterField{};
					std::weak_ptr<mpc::lcdgui::Field> defaultTrackNameFirstLetterField{};
					std::weak_ptr<mpc::lcdgui::Label> defaultTrackNameRestLabel{};
					std::weak_ptr<mpc::lcdgui::Label> trackNameRestLabel{};
					std::weak_ptr<mpc::lcdgui::Field> sq0Field{};
					std::weak_ptr<mpc::lcdgui::Field> sq1Field{};
					std::weak_ptr<mpc::lcdgui::Field> tr0Field{};
					std::weak_ptr<mpc::lcdgui::Field> tr1Field{};
					std::weak_ptr<mpc::lcdgui::Field> displayStyleField{};
					std::weak_ptr<mpc::lcdgui::Field> startTimeField{};
					std::weak_ptr<mpc::lcdgui::Field> hField{};
					std::weak_ptr<mpc::lcdgui::Field> mField{};
					std::weak_ptr<mpc::lcdgui::Field> sField{};
					std::weak_ptr<mpc::lcdgui::Field> fField{};
					std::weak_ptr<mpc::lcdgui::Field> frameRateField{};
					std::vector<std::string> displayStyleNames{};
					std::vector<std::string> frameRateNames{};
					std::weak_ptr<mpc::lcdgui::Field> a0tcField{};
					std::weak_ptr<mpc::lcdgui::Field> a1tcField{};
					std::weak_ptr<mpc::lcdgui::Field> a2tcField{};
					std::weak_ptr<mpc::lcdgui::Field> b0tcField{};
					std::weak_ptr<mpc::lcdgui::Field> b1tcField{};
					std::weak_ptr<mpc::lcdgui::Field> b2tcField{};
					std::weak_ptr<mpc::lcdgui::Field> c0tcField{};
					std::weak_ptr<mpc::lcdgui::Field> c1tcField{};
					std::weak_ptr<mpc::lcdgui::Field> c2tcField{};
					std::weak_ptr<mpc::lcdgui::Field> d0tcField{};
					std::weak_ptr<mpc::lcdgui::Field> d1tcField{};
					std::weak_ptr<mpc::lcdgui::Field> d2tcField{};
					std::weak_ptr<mpc::lcdgui::Field> e0tcField{};
					std::weak_ptr<mpc::lcdgui::Field> e1tcField{};
					std::weak_ptr<mpc::lcdgui::Field> e2tcField{};
					std::weak_ptr<mpc::lcdgui::Field> f0tcField{};
					std::weak_ptr<mpc::lcdgui::Field> f1tcField{};
					std::weak_ptr<mpc::lcdgui::Field> f2tcField{};
					std::weak_ptr<mpc::lcdgui::Field> initialTempoField{};
					std::weak_ptr<mpc::lcdgui::Field> tempoChangeField{};
					std::weak_ptr<mpc::lcdgui::Label> b2tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> c2tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> e2tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> d2tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> f2tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> b1tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> c1tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> d1tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> e1tcLabel{};
					std::weak_ptr<mpc::lcdgui::Label> f1tcLabel{};
					std::vector<std::weak_ptr<mpc::lcdgui::HorizontalBar>> hBars{};
					std::weak_ptr<mpc::lcdgui::Field> noteValueField{};
					std::weak_ptr<mpc::lcdgui::Field> swingField{};
					std::weak_ptr<mpc::lcdgui::Field> notes0Field{};
					std::weak_ptr<mpc::lcdgui::Field> notes1Field{};
					std::weak_ptr<mpc::lcdgui::Field> time0Field{};
					std::weak_ptr<mpc::lcdgui::Field> time1Field{};
					std::weak_ptr<mpc::lcdgui::Field> time2Field{};
					std::weak_ptr<mpc::lcdgui::Field> time3Field{};
					std::weak_ptr<mpc::lcdgui::Field> time4Field{};
					std::weak_ptr<mpc::lcdgui::Field> time5Field{};
					std::weak_ptr<mpc::lcdgui::Field> shiftTimingField{};
					std::weak_ptr<mpc::lcdgui::Field> amountField{};
					std::weak_ptr<mpc::lcdgui::Label> swingLabel{};
					std::weak_ptr<mpc::lcdgui::Label> notes1Label{};
					ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{};
					mpc::lcdgui::FunctionKeys* fb{};
					std::weak_ptr<mpc::lcdgui::Field> bar0Field{};
					std::weak_ptr<mpc::lcdgui::Field> bar1Field{};
					std::weak_ptr<mpc::lcdgui::Field> newTsigField{};
					std::weak_ptr<mpc::lcdgui::Field> countInField{};
					std::weak_ptr<mpc::lcdgui::Field> inPlayField{};
					std::weak_ptr<mpc::lcdgui::Field> rateField{};
					std::weak_ptr<mpc::lcdgui::Field> inRecField{};
					std::weak_ptr<mpc::lcdgui::Field> waitForKeyField{};
					std::vector<std::string> rateNames{};
					std::vector<std::string> countInNames{};
					std::weak_ptr<mpc::lcdgui::Field> numberOfBarsField{};
					std::weak_ptr<mpc::lcdgui::Field> lastBarField{};
					std::weak_ptr<mpc::lcdgui::Field> firstBarField{};
					std::weak_ptr<mpc::lcdgui::Field> changeBarsAfterBarField{};
					std::weak_ptr<mpc::lcdgui::Field> changeBarsNumberOfBarsField{};
					std::weak_ptr<mpc::lcdgui::Field> changeBarsFirstBarField{};
					std::weak_ptr<mpc::lcdgui::Field> changeBarsLastBarField{};
					std::weak_ptr<mpc::lcdgui::Field> inThisTrackField{};
					std::weak_ptr<mpc::lcdgui::Field> newBarsField{};
					std::weak_ptr<mpc::lcdgui::Label> message0Label{};
					std::weak_ptr<mpc::lcdgui::Label> message1Label{};
					std::weak_ptr<mpc::lcdgui::Label> currentLabel{};
					std::weak_ptr<mpc::lcdgui::Field> a0mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> a1mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> a2mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> b0mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> b1mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> b2mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> c0mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> c1mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> c2mrsField{};
					std::weak_ptr<mpc::lcdgui::Field> passField{};
					std::weak_ptr<mpc::lcdgui::Field> typeField{};
					std::weak_ptr<mpc::lcdgui::Field> midiFilterField{};
					std::weak_ptr<mpc::lcdgui::Field> sustainPedalToDurationField{};
					std::weak_ptr<mpc::lcdgui::Field> progChangeSeqField{};
					std::weak_ptr<mpc::lcdgui::Field> receiveChField{};
					std::weak_ptr<mpc::lcdgui::Field> deviceNumberField{};
					std::weak_ptr<mpc::lcdgui::Field> softThruField{};
					std::weak_ptr<mpc::lcdgui::Field> editTypeField{};
					std::weak_ptr<mpc::lcdgui::Field> valueField{};
					std::weak_ptr<mpc::lcdgui::Label> deviceNameLabel{};
					std::weak_ptr<mpc::lcdgui::Field> firstLetterField{};
					mpc::ui::sampler::SamplerGui* samplerGui{};

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
					void displayChangeBarsLastBar();
					void displayChangeBarsFirstBar();
					void displayChangeBarsNumberOfBars();
					void displayChangeBarsAfterBar();
					void displayNumberOfBars();
					void displayLastBar();
					void displayFirstBar();
					void displayWaitForKey();
					void displayInRec();
					void displayRate();
					void displayInPlay();
					void displayCountIn();
					void displayBars();
					void displayNewTsig();
					void displayNoteValue();
					void displaySwing();
					void displayNotes();
					void displayTime();
					void displayShiftTiming();
					void displayAmount();
					void initVisibleEvents();
					void displayInitialTempo();
					void displayTempoChangeOn();
					void displayTempoChange0();
					void displayTempoChange1();
					void displayTempoChange2();
					void displayDisplayStyle();
					void displayStartTime();
					void displayFrameRate();

				public:
					void update(moduru::observer::Observable* o, boost::any arg) override;

				private:
					void displayDeviceName();
					void displayValue();
					void displayEditType();
					void displayTrackNumber();
					void displaySequenceNumberName();
					void displaySequenceNumberNames();
					void displayTrackNumberNames();

				public:
					SequencerWindowObserver(mpc::Mpc* mpc);
					~SequencerWindowObserver();

				};

			}
		}
	}
}

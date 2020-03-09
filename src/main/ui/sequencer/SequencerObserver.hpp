#pragma once
#include <Mpc.hpp>
//#include <lcdgui/LayeredScreen.hpp>
#include <observer/Observer.hpp>
#include <sequencer/Track.hpp>

#include <vector>
#include <string>

namespace mpc {

	class Mpc;

	namespace sequencer {
		class Sequencer;
		class Sequence;
		class Track;
	}

	namespace sampler {
		class Sampler;
	}

	namespace lcdgui {
		class LayeredScreen;
		class Field;
		class Label;
		class BlinkLabel;
	}

	namespace ui {
		namespace sequencer {

			class SequencerObserver
				: public moduru::observer::Observer
			{

			private:
				std::vector<std::string> busNames{};
				mpc::lcdgui::LayeredScreen* ls{};
				mpc::Mpc* mpc;
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				int trackNum{};
				int seqNum{};
				std::weak_ptr<mpc::sequencer::Sequence> seq{};
				std::weak_ptr<mpc::sequencer::Track> track{};
				std::weak_ptr<mpc::lcdgui::Field> trField{};
				std::weak_ptr<mpc::lcdgui::Field> onField{};
				std::weak_ptr<mpc::lcdgui::Field> sqField{};
				std::weak_ptr<mpc::lcdgui::Field> timingField{};
				std::weak_ptr<mpc::lcdgui::Field> countField{};
				std::weak_ptr<mpc::lcdgui::Field> loopField{};
				std::weak_ptr<mpc::lcdgui::Field> recordingModeField{};
				std::weak_ptr<mpc::lcdgui::Field> barsField{};
				std::weak_ptr<mpc::lcdgui::Field> now0Field{};
				std::weak_ptr<mpc::lcdgui::Field> now1Field{};
				std::weak_ptr<mpc::lcdgui::Field> now2Field{};
				std::weak_ptr<mpc::lcdgui::Field> tempoField{};
				std::weak_ptr<mpc::lcdgui::Field> tempoSourceField{};
				std::weak_ptr<mpc::lcdgui::Field> tsigField{};
				std::weak_ptr<mpc::lcdgui::Field> pgmField{};
				std::weak_ptr<mpc::lcdgui::Field> veloField{};
				std::weak_ptr<mpc::lcdgui::Field> busField{};
				std::weak_ptr<mpc::lcdgui::Field> deviceNumberField{};
				std::weak_ptr<mpc::lcdgui::Field> nextSqField{};
				std::weak_ptr<mpc::lcdgui::Label> nextSqLabel{};
				std::weak_ptr<mpc::lcdgui::Label> tempoLabel{};
				std::weak_ptr<mpc::lcdgui::Label> deviceNameLabel{};
				std::weak_ptr<mpc::lcdgui::BlinkLabel> soloLabel{};

			private:
				void displayTempoSource();
				void displaySq();
				void displayVelo();
				void displayDeviceNumber();
				void displayBus();
				void displayBars();
				void displayPgm();
				void displayDeviceName();
				void displayTempo();
				void displayTempoLabel();
				void displayOn();
				void displayTr();
				void displayCount();
				void displayTiming();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			private:
				static std::vector<std::string> timingCorrectNames_;
				void displayNow0();
				void displayNow1();
				void displayNow2();
				void displayRecordingMode();
				void displayTsig();
				void displayLoop();

			public:
				SequencerObserver(Mpc* mpc);
				~SequencerObserver();

			public:
				static std::vector<std::string>* timingCorrectNames();

			};

		}
	}
}

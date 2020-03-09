#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {

	class Mpc;

	namespace sequencer {
		class TimeSignature;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {

		class UserDefaults;

		namespace sequencer {

			class UserObserver
				: public moduru::observer::Observer
			{

			private:
				std::weak_ptr<mpc::ui::UserDefaults> ud{};
				std::vector<std::string> busNames{};
				mpc::Mpc* mpc{};
				mpc::sequencer::TimeSignature* timeSig{};
				std::weak_ptr<mpc::lcdgui::Field> tempoField{};
				std::weak_ptr<mpc::lcdgui::Field> loopField{};
				std::weak_ptr<mpc::lcdgui::Field> tsigField{};
				std::weak_ptr<mpc::lcdgui::Field> barsField{};
				std::weak_ptr<mpc::lcdgui::Field> pgmField{};
				std::weak_ptr<mpc::lcdgui::Field> recordingModeField{};
				std::weak_ptr<mpc::lcdgui::Field> busField{};
				std::weak_ptr<mpc::lcdgui::Field> deviceNumberField{};
				std::weak_ptr<mpc::lcdgui::Label> deviceNameLabel{};
				std::weak_ptr<mpc::lcdgui::Field> veloField{};
				
			private:
				void displayTempo();
				void displayLoop();
				void displayTsig();
				void displayBars();
				void displayPgm();
				void displayRecordingMode();
				void displayBus();
				void displayDeviceNumber();
				void displayVelo();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			private:
				void displayDeviceName();

			public:
				UserObserver(mpc::Mpc* mpc);
				~UserObserver();

			};

		}
	}
}

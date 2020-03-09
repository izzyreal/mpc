#pragma once
#include <observer/Observer.hpp>
#include <Mpc.hpp>

#include <vector>
#include <string>

namespace mpc {

	namespace lcdgui {
		class Field;
	}

	namespace ui {
		namespace midisync {

			class MidiSyncGui;

			class SyncObserver
				: public moduru::observer::Observer
			{

			private:
				MidiSyncGui* msGui{ nullptr };
				std::weak_ptr<mpc::lcdgui::Field> inField{};
				std::weak_ptr<mpc::lcdgui::Field> outField{};
				std::weak_ptr<mpc::lcdgui::Field> modeInField{};
				std::weak_ptr<mpc::lcdgui::Field> modeOutField{};
				std::weak_ptr<mpc::lcdgui::Field> receiveMMCField{};
				std::weak_ptr<mpc::lcdgui::Field> sendMMCField{};
				std::vector<std::string> modeNames{};

			private:
				void displayIn();
				void displayOut();
				void displayModeIn();
				void displayModeOut();
				void displayReceiveMMC();
				void displaySendMMC();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

				SyncObserver(mpc::Mpc* mpc);
				~SyncObserver();

			};

		}
	}
}

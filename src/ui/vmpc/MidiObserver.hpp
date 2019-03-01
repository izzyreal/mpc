#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {

	class Mpc;

	namespace audiomidi {
		class MpcMidiPorts;
	}

	namespace lcdgui {
		class Field;
	}

	namespace ui {

		namespace vmpc {

			class MidiGui;

			class MidiObserver
				: public moduru::observer::Observer
			{

			private:
				MidiGui* midiGui{};
				std::vector<std::string> inNames{};
				std::vector<std::string> outNames{};
				mpc::audiomidi::MpcMidiPorts* mpcMidiPorts{};
				std::weak_ptr<mpc::lcdgui::Field> dev0Field{};
				std::weak_ptr<mpc::lcdgui::Field> dev1Field{};
				std::weak_ptr<mpc::lcdgui::Field> outField{};
				std::weak_ptr<mpc::lcdgui::Field> inField{};

			private:
				void displayDev1();
				void displayOut();
				void displayDev0();
				void displayIn();

			public:
				void update(moduru::observer::Observable* o, std::any a) override;

			public:
				MidiObserver(mpc::Mpc* mpc);
				~MidiObserver();

			};

		}
	}
}

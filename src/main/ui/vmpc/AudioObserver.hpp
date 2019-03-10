#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {

	class Mpc;

	namespace audiomidi {
		class AudioMidiServices;
	}

	namespace lcdgui {
		class Field;
	}


	namespace ui {
		namespace vmpc {

			class AudioGui;

			class AudioObserver
				: public moduru::observer::Observer
			{

			private:
				AudioGui* audioGui{};
				static std::vector<std::string> inNames;
				static std::vector<std::string> outNames;
				std::weak_ptr<mpc::lcdgui::Field> driverField{};
				std::weak_ptr<mpc::lcdgui::Field> dev0Field{};
				std::weak_ptr<mpc::lcdgui::Field> dev1Field{};
				std::weak_ptr<mpc::lcdgui::Field> outField{};
				std::weak_ptr<mpc::lcdgui::Field> inField{};
				std::weak_ptr<mpc::audiomidi::AudioMidiServices> ams{};

			private:
				void displayServer();
				void displayDev1();
				void displayOut();
				void displayDev0();
				void displayIn();

			public:
				void update(moduru::observer::Observable* o, std::any a) override;

			public:
				virtual void displayFunctionKeys();

			public:
				AudioObserver(mpc::Mpc* mpc);
				~AudioObserver();

			};

		}
	}
}

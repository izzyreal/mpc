#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {

	

	namespace lcdgui {
		class Field;
	}

	namespace ui {

		namespace vmpc {

			class DeviceGui;

			class VmpcDiskObserver
				: public moduru::observer::Observer
			{

			private:
				std::weak_ptr<mpc::lcdgui::Field> scsiField{};
				std::weak_ptr<mpc::lcdgui::Field> accessTypeField{};
				std::weak_ptr<mpc::lcdgui::Field> rootField{};
				DeviceGui* deviceGui{};
				

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

			private:
				void displayScsi();
				void displayAccessType();
				void displayRoot();

			public:
				VmpcDiskObserver();
				~VmpcDiskObserver();

			};

		}
	}
}

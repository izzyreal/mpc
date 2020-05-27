#pragma once
#include <observer/Observer.hpp>

#include "VmpcSettingsGui.hpp"

#include <string>
#include <memory>

namespace mpc::lcdgui {
	class Field;
}

namespace mpc::ui::vmpc {

	class VmpcSettingsObserver
		: public moduru::observer::Observer
	{

	private:
		std::vector<std::string> initialPadMappingNames{ "VMPC", "ORIGINAL" };

		std::weak_ptr<mpc::lcdgui::Field> initialPadMappingField;
		VmpcSettingsGui& gui;

	public:
		void update(moduru::observer::Observable* o, nonstd::any a) override;

	private:
		void displayInitialPadMapping();

	public:
		VmpcSettingsObserver();
		~VmpcSettingsObserver();

	};

}

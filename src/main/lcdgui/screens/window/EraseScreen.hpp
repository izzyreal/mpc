#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class EraseScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::vector<std::string> eventClassNames{};

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		EraseScreen(const int& layer);

	};
}

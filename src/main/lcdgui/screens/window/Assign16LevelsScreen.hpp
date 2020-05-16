#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>

namespace mpc::lcdgui::screens::window {

	class Assign16LevelsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		Assign16LevelsScreen(const int& layer);

	};
}

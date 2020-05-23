#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>

namespace mpc::lcdgui::screens {

	class TrimScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void openWindow() override;
		void function(int f) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void pressEnter() override;
		void setSlider(int i) override;

	public:
		TrimScreen(const int layerIndex);
		void open() override;

	};
}

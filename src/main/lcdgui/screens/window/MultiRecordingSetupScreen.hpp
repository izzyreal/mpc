#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class MultiRecordingSetupScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		int yPos = 0;

	public:
		void init() override;

	public:
		void left() override;
		void right() override;
		void up() override;
		void down() override;
		void turnWheel(int i) override;

	public:
		MultiRecordingSetupScreen(const int& layer);

	};
}

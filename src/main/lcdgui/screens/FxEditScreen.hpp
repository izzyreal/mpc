#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class FxEditScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int f) override;
		void turnWheel(int increment) override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;

	private:
		void checkEffects();

	public:
		FxEditScreen(const int layerIndex);
		void open() override;

	private:
		void displayDrum();
		void displayPgm();
		void displayEdit();

	};
}

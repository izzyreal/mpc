#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class Assign16LevelsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;
		void open() override;
		void close() override;
		void openWindow() override;

		Assign16LevelsScreen(mpc::Mpc& mpc, const int layerIndex);
		void update(moduru::observer::Observable* o, nonstd::any msg) override;

	private:
		const std::vector<std::string> typeNames{ "TUNING", "DECAY", "ATTACK", "FILTER" };
		const std::vector<std::string> paramNames{ "VELOCITY", "NOTE VAR" };

		int note = 35;
		int parameter = 0;
		int type = 0;
		int originalKeyPad = 3;

		void displayNote();
		void displayParameter();
		void displayType();
		void displayOriginalKeyPad();

	public:
		void setNote(int newNote);
		void setParam(int i);
		void setType(int i);
		void setOriginalKeyPad(int i);
		int getOriginalKeyPad();
		int getType();
		int getNote();
		int getParameter();

	};
}

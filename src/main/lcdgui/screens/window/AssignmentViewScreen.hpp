#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseSamplerControls.hpp>

namespace mpc::lcdgui::screens::window
{
	class AssignmentViewScreen
		: public mpc::lcdgui::ScreenComponent
		//public mpc::controls::BaseSamplerControls
	{

	public:
		AssignmentViewScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;
		void close() override;
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	public:
		void up() override;
		void down() override;
		void left() override;
		void right() override;
		void turnWheel(int i) override;

	private:
		const std::vector<std::string> letters{ "A", "B", "C", "D" };
		const std::vector<std::string> padFocusNames{ "a3", "b3", "c3", "d3", "a2", "b2", "c2", "d2", "a1", "b1", "c1", "d1", "a0", "b0", "c0", "d0" };
		void displayAssignmentView();
		void displayPad(int i);
		void displayInfo0();
		void displayInfo1();
		void displayInfo2();

		int getPadIndexFromFocus();
		std::string getFocusFromPadIndex();

	};
}

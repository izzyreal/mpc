#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class EditMultipleScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	private:
		int editTypeNumber = 0;

		void checkThreeParameters();
		void checkFiveParameters();
		void checkNotes();

	public:
		EditMultipleScreen(const int layerIndex);
		void open() override;

	private:
		int xPosSingle = 0;
		int yPosSingle = 0;

		std::vector<std::string> noteVariationParameterNames = { "Tun", "Dcy", "Atk", "Flt" };
		std::vector<std::string> editTypeNames = { "ADD VALUE", "SUB VALUE", "MULT VAL%", "SET TO VAL" };
		std::vector<std::string> singleLabels = { "Change note to:", "Variation type:", "Variation value:" };
		std::vector<int> xPosDouble = { 60, 84 };
		std::vector<int> yPosDouble = { 22, 33 };
		std::vector<std::string> doubleLabels = { "Edit type:", "Value:" };

		void updateEditMultiple();
		void updateDouble();
		void setEditTypeNumber(int i);

	};
}

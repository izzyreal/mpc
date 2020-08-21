#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class StepEditorScreen;
}

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
		EditMultipleScreen(mpc::Mpc& mpc, const int layerIndex);
		void open() override;
		void close() override;
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	private:
		std::vector<std::string> noteVariationParameterNames = { "Tun", "Dcy", "Atk", "Flt" };
		std::vector<std::string> editTypeNames = { "ADD VALUE", "SUB VALUE", "MULT VAL%", "SET TO VAL" };
		std::vector<std::string> singleLabels = { "Change note to:", "Variation type:", "Variation value:" };
		const int xPosSingle = 60;
		const int yPosSingle = 25;
		std::vector<int> xPosDouble = { 60, 84 };
		std::vector<int> yPosDouble = { 22, 33 };
		std::vector<std::string> doubleLabels = { "Edit type:", "Value:" };

		int changeNoteToNumber = 35;
		int changeVariationTypeNumber = 0;
		int changeVariationValue = 0;
		int editValue = 0;

		void updateEditMultiple();
		void updateDouble();
		void setEditTypeNumber(int i);

		void setChangeNoteToIndex(int i);
		void setChangeVariationTypeNumber(int i);
		void setChangeVariationValue(int i);
		void setEditValue(int i);

		friend class mpc::lcdgui::screens::StepEditorScreen;

	};
}

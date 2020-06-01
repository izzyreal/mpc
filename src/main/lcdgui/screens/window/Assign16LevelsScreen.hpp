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

	public:
		Assign16LevelsScreen(const int layerIndex);

	private:
		int note = 35;
		int parameter = 0;
		int type = 0;
		int originalKeyPad = 0;
	
	private:
		static std::vector<std::string> TYPE_NAMES;
		static  std::vector<std::string> PARAM_NAMES;

	public:
		void setNote(int newNote);
		void setParameter(int i);
		void setType(int i);
		void setOriginalKeyPad(int i);
		int getOriginalKeyPad();
		int getType();
		int getNote();
		int getParameter();

	private:
		void displayNote();
		void displayParameter();
		void displayType();
		void displayOriginalKeyPad();

	};
}

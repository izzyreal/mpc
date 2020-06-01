#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <file/all/AllParser.hpp>

namespace mpc::lcdgui::screens::window
{
	class SaveAllFileScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::unique_ptr<mpc::file::all::AllParser> allParser{};

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		void open() override;

		SaveAllFileScreen(const int layerIndex);

	private:
		void displayFile();

	};
}

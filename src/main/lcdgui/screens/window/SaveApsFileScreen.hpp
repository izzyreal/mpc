#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <disk/ApsSaver.hpp>

namespace mpc::lcdgui::screens::window
{
	class SaveApsFileScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::unique_ptr<mpc::disk::ApsSaver> apsSaver;

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		SaveApsFileScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		const std::vector<std::string> apsSaveNames{ "APS ONLY", "WITH SOUNDS", "WITH .WAV" };

		void displayFile();
		void displaySave();
		void displayReplaceSameSounds();

	};
}

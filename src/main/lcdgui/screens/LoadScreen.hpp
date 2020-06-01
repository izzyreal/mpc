#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc
{
	class Mpc;
}

namespace mpc::controls::other::dialog
{
	class NameControls;
}

namespace mpc::disk
{
	class AbstractDisk;
	class StdDisk;
}

namespace mpc::lcdgui::screens::window
{
	class DirectoryScreen;
	class LoadApsFileScreen;
	class LoadASequenceFromAllScreen;
	class Mpc2000XlAllFileScreen;
	class LoadASoundScreen;
}

namespace mpc::lcdgui::screens
{
	class LoadScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void openWindow() override;
		void turnWheel(int i) override;

		LoadScreen(const int layerIndex);
		
		void open() override;

	private:
		const std::vector<std::string> views{ "All Files", ".SND", ".PGM", ".APS", ".MID", ".ALL", ".WAV", ".SEQ", ".SET" };

		int view = 0;
		int fileLoad = 0;

		void displayView();
		void displayDirectory();
		void displayFreeSnd();
		void displayFile();
		void displaySize();

		void setView(int i);
		mpc::disk::MpcFile* getSelectedFile();
		std::string getSelectedFileName();
		bool isSelectedFileDirectory();
		void setFileLoadWithMaxCheck(int i);
		void setFileLoad(int i);

	private:
		friend class mpc::lcdgui::screens::window::DirectoryScreen;
		friend class mpc::lcdgui::screens::window::LoadApsFileScreen;
		friend class mpc::lcdgui::screens::window::LoadASequenceFromAllScreen;
		friend class mpc::lcdgui::screens::window::Mpc2000XlAllFileScreen;
		friend class mpc::lcdgui::screens::window::LoadASoundScreen;
		friend class mpc::controls::BaseControls;
		friend class mpc::controls::other::dialog::NameControls;
		friend class mpc::disk::AbstractDisk;
		friend class mpc::disk::StdDisk;
		friend class mpc::Mpc;
	};
}

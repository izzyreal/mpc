#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class NameScreen;
}

namespace mpc::lcdgui::screens
{
	class LoadScreen;
    class SaveScreen;
}

namespace mpc::lcdgui::screens::dialog2
{
	class DeleteAllFilesScreen;
}

namespace mpc::lcdgui::screens::dialog
{
	class DeleteFileScreen;
	class DeleteFolderScreen;
}

namespace mpc::disk
{
	class AbstractDisk;
    class MpcFile;
}

namespace mpc::lcdgui::screens::window
{
	class DirectoryScreen
		: public mpc::lcdgui::ScreenComponent
	{
	public:
        DirectoryScreen(mpc::Mpc& mpc, const int layerIndex);

        void function(int f) override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;
		void turnWheel(int i) override;
		void open() override;

	private:
		int xPos = 0;
		int yPos0 = 0;
		int yOffset0 = 0;
		int yOffset1 = 0;

		void setFunctionKeys();
		std::shared_ptr<mpc::disk::MpcFile> getSelectedFile();
        std::shared_ptr<mpc::disk::MpcFile> getFileFromGrid(int x, int y);
		void displayLeftFields();
		void displayRightFields();
		void refreshFocus();

		std::vector<std::string> getFirstColumn();
		std::vector<std::string> getSecondColumn();
		int getXPos();
		int getYpos0();
		void findYOffset0();
		void setYOffset0(int i);
		void setYOffset1(int i);
		void setYPos0(int i);
		std::string padFileName(std::string s, std::string pad);
		void drawGraphicsLeft();
		void drawGraphicsRight();

		friend class mpc::disk::AbstractDisk;
		friend class mpc::lcdgui::screens::LoadScreen;
		friend class mpc::lcdgui::screens::SaveScreen;
		friend class mpc::lcdgui::screens::window::NameScreen;
		friend class mpc::lcdgui::screens::dialog2::DeleteAllFilesScreen;
		friend class mpc::lcdgui::screens::dialog::DeleteFileScreen;
		friend class mpc::lcdgui::screens::dialog::DeleteFolderScreen;

	};
}

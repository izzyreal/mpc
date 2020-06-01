#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::controls::other::dialog
{
	class NameControls;
}

namespace mpc::lcdgui::screens
{
	class LoadScreen;
}

namespace mpc::disk
{
	class AbstractDisk;
}

namespace mpc::lcdgui::screens::window
{
	class DirectoryScreen
		: public mpc::lcdgui::ScreenComponent
	{
	public:
		void function(int f) override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;
		void turnWheel(int i) override;

		DirectoryScreen(const int layerIndex);

	private:
		int xPos = 0;
		int yPos0 = 0;
		int yOffset0 = 0;
		int yOffset1 = 0;
		std::string previousScreenName = "";

	private:
		mpc::disk::MpcFile* getSelectedFile();
		mpc::disk::MpcFile* getFileFromGrid(int x, int y);
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

	private:
		friend class mpc::controls::BaseControls;
		friend class mpc::controls::other::dialog::NameControls;
		friend class mpc::disk::AbstractDisk;
		friend class mpc::lcdgui::screens::LoadScreen;

	};
}

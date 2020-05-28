#pragma once

#include <observer/Observable.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {

	
	
	namespace disk {
		class MpcFile;
	}

	namespace lcdgui {
		class Field;
	}


	namespace ui {

		namespace disk {

			class DiskGui;

			namespace window {

				class DirectoryGui
					: public moduru::observer::Observable
				{

				private:
					
					mpc::ui::disk::DiskGui* diskGui{ nullptr };
					int xPos{ 0 };
					int yPos0{ 0 };
					int yOffset0{ 0 };
					int yOffset1{ 0 };
					std::string previousScreenName = "";

				public:
					void left();
					void right();
					void up();
					void down();
					mpc::disk::MpcFile* getSelectedFile();
					mpc::disk::MpcFile* getFileFromGrid(int x, int y);
					void displayLeftFields(std::vector<std::weak_ptr<mpc::lcdgui::Field>> tfa);
					void displayRightFields(std::vector<std::weak_ptr<mpc::lcdgui::Field>> tfa);
					void refreshFocus(std::vector<std::weak_ptr<mpc::lcdgui::Field>> tfa0, std::vector<std::weak_ptr<mpc::lcdgui::Field>> tfa1);
					int getYOffsetFirst();
					int getYOffsetSecond();
					std::vector<std::string> getFirstColumn();
					std::vector<std::string> getSecondColumn();
					int getXPos();
					int getYpos0();
					void setPreviousScreenName(std::string s);
					std::string getPreviousScreenName();
					void findYOffset0();
					void setYOffset0(int i);
					void setYOffset1(int i);
					void setYPos0(int i);

					DirectoryGui(mpc::ui::disk::DiskGui* diskGui);
					~DirectoryGui();

				};

			}
		}
	}
}

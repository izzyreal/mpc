#pragma once

#include <observer/Observer.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mpc {
	
	namespace disk {
		class AbstractDisk;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {

		namespace disk {
			namespace window {

				class DirectoryGui;

				class DirectoryObserver
					: public moduru::observer::Observer
				{

				private:
					DirectoryGui* directoryGui{ nullptr };
					std::weak_ptr<mpc::disk::AbstractDisk> disk{};
					std::vector<std::weak_ptr<mpc::lcdgui::Field>> left{};
					std::vector<std::weak_ptr<mpc::lcdgui::Field>> right{};
					
					std::weak_ptr<mpc::lcdgui::Field> a0Field{};
					std::weak_ptr<mpc::lcdgui::Field> a1Field{};
					std::weak_ptr<mpc::lcdgui::Field> a2Field{};
					std::weak_ptr<mpc::lcdgui::Field> a3Field{};
					std::weak_ptr<mpc::lcdgui::Field> a4Field{};
					std::weak_ptr<mpc::lcdgui::Field> b0Field{};
					std::weak_ptr<mpc::lcdgui::Field> b1Field{};
					std::weak_ptr<mpc::lcdgui::Field> b2Field{};
					std::weak_ptr<mpc::lcdgui::Field> b3Field{};
					std::weak_ptr<mpc::lcdgui::Field> b4Field{};
					std::weak_ptr<mpc::lcdgui::Label> topLeftLabel{};
					std::weak_ptr<mpc::lcdgui::Label> a0Label{};
					std::weak_ptr<mpc::lcdgui::Label> a1Label{};
					std::weak_ptr<mpc::lcdgui::Label> a2Label{};
					std::weak_ptr<mpc::lcdgui::Label> a3Label{};
					std::weak_ptr<mpc::lcdgui::Label> a4Label{};
					std::weak_ptr<mpc::lcdgui::Label> b0Label{};
					std::weak_ptr<mpc::lcdgui::Label> b1Label{};
					std::weak_ptr<mpc::lcdgui::Label> b2Label{};
					std::weak_ptr<mpc::lcdgui::Label> b3Label{};
					std::weak_ptr<mpc::lcdgui::Label> b4Label{};
					std::weak_ptr<mpc::lcdgui::Label> c0Label{};
					std::weak_ptr<mpc::lcdgui::Label> c1Label{};
					std::weak_ptr<mpc::lcdgui::Label> c2Label{};
					std::weak_ptr<mpc::lcdgui::Label> c3Label{};
					std::weak_ptr<mpc::lcdgui::Label> c4Label{};

				private:

					std::string padFileName(std::string s, std::string pad);

					void updateLeft();
					void updateRight();

				public:
					void update(moduru::observer::Observable* o, nonstd::any a) override;

				private:
					void updateFocus();
					void drawGraphicsLeft();
					void drawGraphicsRight();

				public:
					DirectoryObserver(std::weak_ptr<mpc::disk::AbstractDisk> disk);
					~DirectoryObserver();
				};

			}
		}
	}
}

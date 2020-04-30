#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {

	

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {

		namespace disk {

			class DiskGui;

			namespace window {

				class SaveAllFileObserver
					: public moduru::observer::Observer
				{

				private:
					
					std::weak_ptr<mpc::lcdgui::Field> fileField{};
					std::weak_ptr<mpc::lcdgui::Label> fileLabel{};
					mpc::ui::disk::DiskGui* diskGui{ nullptr };

				private:
					void displayFile();

				public:
					void update(moduru::observer::Observable* o, nonstd::any a) override;

					SaveAllFileObserver();
					~SaveAllFileObserver();
				};

			}
		}
	}
}

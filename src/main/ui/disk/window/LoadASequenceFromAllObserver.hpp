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
				class DiskWindowGui;
			}

			class LoadASequenceFromAllObserver
				: public moduru::observer::Observer
			{

			private:
				
				std::weak_ptr<mpc::lcdgui::Field> fileField{};
				std::weak_ptr<mpc::lcdgui::Label> fileLabel{};
				std::weak_ptr<mpc::lcdgui::Field> loadIntoField{};
				std::weak_ptr<mpc::lcdgui::Label> loadIntoLabel{};
				DiskGui* diskGui{ nullptr };
				mpc::ui::disk::window::DiskWindowGui* diskWindowGui{ nullptr };

			private:
				void displayFile();
				void displayLoadInto();

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

				LoadASequenceFromAllObserver();
				~LoadASequenceFromAllObserver();
			};

		}
	}
}

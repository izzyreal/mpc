#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	
	class Mpc;

	namespace lcdgui {
		class Field;
	}

	namespace ui {
		namespace disk {
			namespace window {

				class DiskWindowGui;

				class DeleteAllFilesObserver
					: public moduru::observer::Observer
				{

				private:
					std::vector<std::string> views{};
					std::string csn{ "" };
					DiskWindowGui* diskWindowGui{ nullptr };
	//				mpc::lcdgui::LayeredScreen* ls{ nullptr };
					std::weak_ptr<mpc::lcdgui::Field> deleteField{};

				private:
					void displayDelete();

				public:
					void update(moduru::observer::Observable* o, std::any a) override;

					DeleteAllFilesObserver(mpc::Mpc* mpc);
					~DeleteAllFilesObserver();
				};

			}
		}
	}
}

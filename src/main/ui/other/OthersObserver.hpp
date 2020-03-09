#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {
	class Mpc;
	namespace lcdgui {
		class Field;
	}

	namespace ui {
		namespace other {

			class OthersGui;

			class OthersObserver
				: public moduru::observer::Observer
			{

			private:
				std::weak_ptr<mpc::lcdgui::Field> tapAveragingField{};
				OthersGui* othersGui{ nullptr };

			private:
				void displayTapAveraging();

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

			public:
				OthersObserver(mpc::Mpc* mpc);
				~OthersObserver();
			};

		}
	}
}

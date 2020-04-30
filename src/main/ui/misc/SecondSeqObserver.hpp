#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {
	
	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {
		namespace misc {

			class SecondSeqGui;

			class SecondSeqObserver
				: public moduru::observer::Observer
			{

			private:
				
				std::weak_ptr<mpc::lcdgui::Field> sqField{};
				std::weak_ptr<mpc::lcdgui::Label> sequenceNameLabel{};
				SecondSeqGui* secondSeqGui{ nullptr };

			private:
				void displaySq();

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

				SecondSeqObserver();
				~SecondSeqObserver();
			
			};

		}
	}
}

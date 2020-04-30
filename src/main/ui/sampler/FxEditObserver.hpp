#pragma once
#include <observer/Observer.hpp>

#include <vector>
#include <memory>

namespace mpc {

	

	namespace lcdgui {
		class Field;
		class Effect;
	}

	namespace ui {

		namespace sampler {

			class MixerSetupGui;

			class FxEditObserver
				: public moduru::observer::Observer
			{

			private:
				


			private:
				std::weak_ptr<lcdgui::Field> drumField;
				std::weak_ptr<lcdgui::Field> pgmField;
				std::weak_ptr<lcdgui::Field> editField;

				MixerSetupGui* msGui = nullptr;

				std::vector<std::weak_ptr<lcdgui::Effect>> effects;

			private:
				void displayDrum();
				void displayPgm();
				void displayEdit();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			public:
				FxEditObserver();
				~FxEditObserver();

			};

		}
	}
}

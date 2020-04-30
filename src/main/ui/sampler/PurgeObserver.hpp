#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {

	

	namespace sampler {
		class Sampler;
	}

	namespace lcdgui {
		class Label;
	}

	namespace ui {
		namespace sampler {

			class PurgeObserver
				: public moduru::observer::Observer
			{

			private:
				
				std::weak_ptr<mpc::sampler::Sampler> sampler{ };
				std::weak_ptr<mpc::lcdgui::Label> valueLabel{};

			private:
				void displayValue();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

				PurgeObserver();
				~PurgeObserver();

			};

		}
	}
}

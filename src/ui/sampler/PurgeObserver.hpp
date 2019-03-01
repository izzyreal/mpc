#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {

	class Mpc;

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
				mpc::Mpc* mpc{ nullptr };
				std::weak_ptr<mpc::sampler::Sampler> sampler{ };
				std::weak_ptr<mpc::lcdgui::Label> valueLabel{};

			private:
				void displayValue();

			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

				PurgeObserver(mpc::Mpc* mpc);
				~PurgeObserver();

			};

		}
	}
}

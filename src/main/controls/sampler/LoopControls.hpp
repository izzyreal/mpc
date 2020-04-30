#pragma once
#include <controls/sampler/AbstractSamplerControls.hpp>

namespace mpc {
	namespace controls {
		namespace sampler {

			class LoopControls
				: public AbstractSamplerControls
			{

			public:
				typedef AbstractSamplerControls super;

			public:
				void init() override;

			public:
				void openWindow() override;
				void function(int f) override;
				void turnWheel(int i) override;
				void setSlider(int i) override;
				void left() override;
				void right() override;
				void pressEnter() override;

				LoopControls();

			};

		}
	}
}

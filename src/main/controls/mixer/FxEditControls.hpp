#pragma once
#include <controls/mixer/AbstractMixerControls.hpp>

namespace mpc {
	namespace controls {
		namespace mixer {

			class FxEditControls
				: public AbstractMixerControls
			{

			public:
				typedef AbstractMixerControls super;
				void function(int f) override;
				void turnWheel(int increment) override;
				void left() override;
				void right() override;
				void up() override;
				void down() override;

			private:
				void checkEffects();

			public:
				FxEditControls();
				~FxEditControls();

			};

		}
	}
}

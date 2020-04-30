#pragma once
#include <controls/mixer/AbstractMixerControls.hpp>

namespace mpc {
	namespace controls {
		namespace mixer {

			class MixerControls
				: public AbstractMixerControls
			{

			public:
				typedef AbstractMixerControls super;
				void up() override;
				void down() override;
				void left() override;
				void right() override;
				void openWindow() override;
				void function(int f) override;
				void turnWheel(int increment) override;
				void pad(int i, int velo, bool repeat, int tick) override;

			private:
				void recordMixerEvent(int pad, int param, int value);

			public:
				MixerControls();
				~MixerControls();

			};

		}
	}
}

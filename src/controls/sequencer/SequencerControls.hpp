#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {

	class Mpc;

	namespace controls {
		namespace sequencer {

			class SequencerControls
				: public AbstractSequencerControls
			{

			public:
				typedef AbstractSequencerControls super;

			public:
				void init() override;

			public:
				void pressEnter() override;
				void function(int i) override;
				void turnWheel(int i) override;
				void openWindow() override;
				void left() override;
				void right() override;
				void up() override;
				void down() override;

				SequencerControls(mpc::Mpc* mpc);

			};

		}
	}
}

#pragma once
#include <controls/vmpc/AbstractVmpcControls.hpp>

namespace mpc {
	namespace controls {
		namespace vmpc {

			class MidiControls
				: public AbstractVmpcControls
			{

			public:
				typedef AbstractVmpcControls super;
				void function(int i) override;
				void turnWheel(int i) override;
				void up() override;
				void down() override;

			public:
				MidiControls(mpc::Mpc* mpc);

			};

		}
	}
}

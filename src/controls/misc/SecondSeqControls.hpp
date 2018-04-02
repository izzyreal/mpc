#pragma once
#include <controls/misc/AbstractMiscControls.hpp>

namespace mpc {
	class Mpc;
	namespace controls {
		namespace misc {

			class SecondSeqControls
				: public AbstractMiscControls
			{

			public:
				typedef AbstractMiscControls super;
				void function(int i) override;
				void turnWheel(int i) override;

			public:
				SecondSeqControls(mpc::Mpc* mpc);
				~SecondSeqControls();

			};

		}
	}
}

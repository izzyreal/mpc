#pragma once
#include <controls/other/AbstractOtherControls.hpp>

namespace mpc {
	namespace controls {
		namespace other {

			class VerControls
				: public AbstractOtherControls
			{

			public:
				typedef AbstractOtherControls super;
				void function(int i) override;

			public:
				VerControls(mpc::Mpc* mpc) ;
				~VerControls();
			};

		}
	}
}

#pragma once
#include <controls/other/AbstractOtherControls.hpp>

namespace mpc {
	namespace controls {
		namespace other {

			class InitControls
				: public mpc::controls::other::AbstractOtherControls
			{

			public:
				typedef mpc::controls::other::AbstractOtherControls super;
				void function(int i) override;

				InitControls() ;
			};

		}
	}
}

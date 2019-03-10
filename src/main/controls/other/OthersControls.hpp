#pragma once
#include <controls/AbstractControls.hpp>

namespace mpc {

	namespace ui {
		namespace other {
			class OthersGui;
		}
	}

	namespace controls {
		namespace other {

			class OthersControls
				: public mpc::controls::AbstractControls
			{

			private:
				mpc::ui::other::OthersGui* othersGui;

			public:
				typedef mpc::controls::AbstractControls super;
				void function(int i) override;
				void turnWheel(int i) override;

			public:
				OthersControls(mpc::Mpc* mpc) ;
				~OthersControls();
			};

		}
	}
}

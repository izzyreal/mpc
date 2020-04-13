#pragma once
#include <controls/BaseControls.hpp>

namespace mpc {

	namespace ui {
		namespace other {
			class OthersGui;
		}
	}

	namespace controls {
		namespace other {

			class OthersControls
				: public mpc::controls::BaseControls
			{

			private:
				mpc::ui::other::OthersGui* othersGui;

			public:
				typedef mpc::controls::BaseControls super;
				void function(int i) override;
				void turnWheel(int i) override;

			public:
				OthersControls(mpc::Mpc* mpc) ;
				~OthersControls();
			};

		}
	}
}

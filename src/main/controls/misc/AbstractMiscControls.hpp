#pragma once
#include <controls/BaseControls.hpp>

namespace mpc {
	
	namespace ui {
		namespace misc {
			class PunchGui;
			class TransGui;
			class SecondSeqGui;
		}
	}

	namespace controls {
		namespace misc {

			class AbstractMiscControls
				: public mpc::controls::BaseControls
			{

			public:
				typedef mpc::controls::BaseControls super;

			protected:
				mpc::ui::misc::PunchGui* punchGui{ nullptr };
				mpc::ui::misc::TransGui* transGui{ nullptr };
				mpc::ui::misc::SecondSeqGui* secondSeqGui{ nullptr };

			public:
				AbstractMiscControls();
				virtual ~AbstractMiscControls();

			};

		}
	}
}

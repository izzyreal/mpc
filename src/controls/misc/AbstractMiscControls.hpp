#pragma once
#include <controls/AbstractControls.hpp>

namespace mpc {
	class Mpc;
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
				: public mpc::controls::AbstractControls
			{

			public:
				typedef mpc::controls::AbstractControls super;

			protected:
				mpc::ui::misc::PunchGui* punchGui{ nullptr };
				mpc::ui::misc::TransGui* transGui{ nullptr };
				mpc::ui::misc::SecondSeqGui* secondSeqGui{ nullptr };

			public:
				AbstractMiscControls(mpc::Mpc* mpc);
				virtual ~AbstractMiscControls();

			};

		}
	}
}

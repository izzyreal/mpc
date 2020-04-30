#pragma once
#include <controls/other/AbstractOtherControls.hpp>

namespace mpc {
	namespace controls {
		namespace other {
			namespace dialog {

				class NameControls
					: public mpc::controls::other::AbstractOtherControls
				{

				public:
					void left() override;
					void right() override;
					void turnWheel(int j) override;
					void function(int i) override;
					void pressEnter() override;

				private:
					void drawUnderline();
					void initEditColors();
					void resetNameGui();
					void saveName();

				public:
					NameControls();
					~NameControls();

				};

			}
		}
	}
}

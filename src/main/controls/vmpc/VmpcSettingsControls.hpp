#pragma once
#include <controls/BaseControls.hpp>

namespace mpc::controls::vmpc {

	class VmpcSettingsControls
		: public mpc::controls::BaseControls
	{

	public:
		void turnWheel(int i) override;

	};
}

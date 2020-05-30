#pragma once
#include "BaseControls.hpp"

namespace mpc::controls {

	class BaseSamplerControls
		: virtual public BaseControls
	{

	protected:
		void init() override;

	};
}

#pragma once

#include "Component.hpp"

#include <memory>
#include <string>

namespace mpc::lcdgui
{

	class ScreenComponent
		: public Component
	{

	public:
		ScreenComponent(const std::string& name);

	};

}

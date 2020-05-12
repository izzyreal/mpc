#pragma once

#include "Component.hpp"

#include <observer/Observer.hpp>

#include <memory>
#include <string>

namespace mpc::lcdgui
{

	class ScreenComponent
		: public Component, public moduru::observer::Observer
	{

	public:
		ScreenComponent(const std::string& name);

	};

}

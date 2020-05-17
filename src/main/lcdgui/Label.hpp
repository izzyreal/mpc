#pragma once
#include <string>

#include "TextComp.hpp"
#include <gui/BMFParser.hpp>

namespace mpc::lcdgui {

	class Label
		: public TextComp

	{

	public:
		Label(const std::string& name, std::string text, int x, int y, int width);

	};
}

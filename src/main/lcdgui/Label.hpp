#pragma once
#include <string>

#include "TextComp.hpp"

namespace mpc { class Mpc; }

namespace mpc::lcdgui {
	class Label : public TextComp
	{
	public:
		Label(mpc::Mpc& mpc, const std::string& name, std::string text, int x, int y, int width);
	};
}

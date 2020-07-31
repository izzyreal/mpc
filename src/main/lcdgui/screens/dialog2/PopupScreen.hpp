#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <string>

namespace mpc::lcdgui::screens::dialog2
{
	class PopupScreen
		: public ScreenComponent
	{

	public:
		void setText(std::string text);

	public:
		PopupScreen(mpc::Mpc& mpc);

	};
}

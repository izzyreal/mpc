#pragma once
#include "Parameter.hpp"
#include "Component.hpp"
#include "Info.hpp"
#include "Background.hpp"
#include "FunctionKeys.hpp"

#include <string>
#include <memory>

namespace mpc::lcdgui
{

	class Layer
		: public Component
	{

	private:
		std::string focus = "";

	public:
		mpc::lcdgui::Background* getBackground();
		mpc::lcdgui::FunctionKeys* getFunctionKeys();
		void setFocus(std::string textFieldName);
		std::string getFocus();

	public:
		Layer();

	};
}

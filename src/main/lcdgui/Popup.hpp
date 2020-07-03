#pragma once
#include "Component.hpp"

#include <string>
#include <memory>

namespace mpc::lcdgui
{
	class Background;

	class Popup
		: public Component
	{

	private:
		std::string text = "";

	public:
		void setText(std::string text);

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		Popup();

	};
}

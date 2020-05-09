#pragma once
#include <string>

#include "TextComp.hpp"
#include <gui/BMFParser.hpp>

namespace mpc::lcdgui {

	class Label
		: public TextComp

	{
	private:
		bool rigorousClearing = false;

	public:
		void enableRigorousClearing();

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		Label(const std::string& name, std::string text, int x, int y, int columns);
		~Label() override;

	};
}

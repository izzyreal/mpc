#pragma once
#include "Component.hpp"

#include <gui/BMFParser.hpp>

#include <string>
#include <memory>

namespace mpc {
	namespace lcdgui {

		class Background;

		class Popup
			: public Component			
		{

		private:
			std::vector<std::vector<bool>>* atlas;
			moduru::gui::bmfont* font;
			int pos{ 0 };
			std::string text{ "" };
			std::unique_ptr<Background> bg;

		public:
			void setText(std::string text, int pos);

		public:
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			Popup(std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font);
			~Popup();

		};

	}
}

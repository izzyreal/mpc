#pragma once

#include <gui/BMFParser.hpp>
#include "TextComp.hpp"

#include <memory>
#include <cstdint>
#include <string>
#include <vector>

namespace mpc {

	namespace lcdgui {

		class LayeredScreen;

		class Label;
		class Field
			: public TextComp
		{

		private:
			mpc::lcdgui::LayeredScreen* layeredScreen{};
			std::vector<Label*> letters{};

			bool focusable{ true };
			bool focus{ false };
			std::string csn{ "" };
			bool split{ false };
			int activeSplit{ 0 };
			bool typeModeEnabled{ false };
			std::string oldText{ "" };
			bool scrolling{ false };
			//Field_Scroller* scroller{};
			static const int BLINKING_RATE{ 200 };
			bool blinking{ false };

		public:
			void setSize(int x, int y) override;
			void setText(std::string s) override;
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			bool hasFocus();
			void setFocusable(bool b);
			bool isFocusable();
			void loseFocus(std::string next);
			void takeFocus(std::string prev);
			void setSplit(bool b);
			void redrawSplit();
			bool isSplit();
			int getActiveSplit();
			bool setActiveSplit(int i);
			bool enableTypeMode();
			int enter();
			void type(int i);
			bool isTypeModeEnabled();
			void disableTypeMode();
			//void enableScrolling(std::vector<CLabel> enablers);
			void startBlinking();
			void stopBlinking();
			bool getBlinking();

			void initialize(std::string name, int x, int y, int columns);

		public:
			Field(mpc::lcdgui::LayeredScreen* layeredScreen, std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font);
			~Field();

		};
	}
}

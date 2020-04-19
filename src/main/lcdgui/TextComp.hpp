#pragma once
#include "Component.hpp"
#include <gui/BMFParser.hpp>

#include <string>
#include <vector>
#include <memory>
#include <thread>

namespace mpc {
	namespace lcdgui {
		class TextComp
			: public Component

		{
		private:
			std::vector<std::vector<bool>>* atlas;
			moduru::gui::bmfont* font;

		protected:
			bool inverted{ false };
			bool noLeftMargin{ false };

		protected:
			unsigned int x{ 0 };
			unsigned int y{ 0 };
			unsigned int w{ 0 };
			unsigned int h{ 0 };

			bool scrolling{ false };
			std::thread scrollThread;

		protected:
			const int TEXT_HEIGHT = 7;
			const int TEXT_WIDTH = 6;

		protected:
			std::string name{ "" };
			std::	string text{ "" };
			int columns{ 0 }; // characters
			bool opaque{ false };

		private:
			void initRECT();
			void scroll();

		private:
			static void static_scroll(void * args);

		public:
			virtual void setSize(int width, int height, bool clear = true);
			virtual void setLocation(int x, int y, bool clear = true);
			virtual void setText(const std::string& s);

		public:
			void setOpaque(bool b);
			void setInverted(bool b);
			void setNoLeftMargin(bool b);
			int getX();
			int getY();
			int getW();
			int getH();
			std::string getText();
			int getColumns();
			void setColumns(int i);
			std::string getName();
			void setTextPadded(std::string s, std::string padding);
			void setTextPadded(int i, std::string padding);
			void setScrolling(bool scrolling);
			unsigned int GetTextEntryLength();

		public:
			void Draw(std::vector<std::vector<bool> >* pixels) override;
			void Hide(bool b) override;

		public:
			TextComp(std::vector<std::vector<bool>>* atlas,	moduru::gui::bmfont* font);
			virtual ~TextComp();

		};

	}
}

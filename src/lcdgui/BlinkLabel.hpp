#pragma once
#include <lcdgui/Label.hpp>

#include <gui/BMFParser.hpp>

#include <thread>
#include <string>

namespace mpc {
	namespace lcdgui {
		class BlinkLabel
			: public Label
		{

		private:
			int BLINK_INTERVAL{ 500 };
			bool blinking{ false };
			std::thread blinkThread;

		private:
			static void static_blink(void* args);
			void runBlinkThread();

		public:
			void setBlinking(bool flag);
			bool isBlinking();

		public:
			void Hide(bool b) override;
			void initialize(std::string name, std::string text, int x, int y, int columns) override;

		public:
			BlinkLabel(std::string text, std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font);
			~BlinkLabel();

		};

	}
}

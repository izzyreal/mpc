#pragma once
#include "Component.hpp"
#include <gui/BMFParser.hpp>

#include <string>
#include <vector>
#include <memory>
#include <thread>

namespace mpc::lcdgui
{

	class TextComp
		: public Component
	{
	private:
		static const int BLINK_INTERVAL = 300;
		bool blinking = false;
		std::thread blinkThread;
		static void static_blink(void* args);
		void runBlinkThread();

	protected:
		bool inverted = false;
		const int FONT_HEIGHT = 7;
		const int FONT_WIDTH = 6;

	protected:
		std::string text = "";

	public:
		virtual void setText(const std::string& s);

	public:
		void setBlinking(bool b);
		void setInverted(bool b);
		int getX();
		int getY();
		int getW();
		int getH();
		std::string getText();
		std::string getName();
		void setTextPadded(std::string s, std::string padding);
		void setTextPadded(int i, std::string padding);
		unsigned int GetTextEntryLength();

	public:
		virtual void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		TextComp(const std::string& name);
		~TextComp();

	};
}

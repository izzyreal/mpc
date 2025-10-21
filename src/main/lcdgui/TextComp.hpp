#pragma once
#include "Component.hpp"

#include <string>
#include <vector>
#include <memory>
#include <thread>

namespace mpc { class Mpc; }

namespace mpc::lcdgui
{

	enum class Alignment { None, Centered };

	class TextComp
		: public Component
	{
	private:
		mpc::Mpc& mpc;
		static const int BLINK_INTERVAL = 300;
		bool blinking = false;
        bool invisibleDueToBlinking = false;
		std::thread blinkThread;
		static void static_blink(void* args);
		void runBlinkThread();
		Alignment alignment = Alignment::None;
		bool textuallyAligned = false;
		int alignmentEndX = -1;
		bool twoDots = false;
		int leftMargin = 0;

	protected:
		bool inverted = false;
		bool doubleInverted = false;
		const int FONT_HEIGHT = 7;
		const int FONT_WIDTH = 6;
        bool isInverted();

	protected:
		std::string text;

	public:
		virtual void setText(const std::string& s);

	public:
		void setLeftMargin(const int margin);
		void enableTwoDots();
		void setBlinking(bool b);
		void setInverted(bool b);
		void setDoubleInverted(bool b);
		void setAlignment(const Alignment newAlignment, int endX = -1);
		int getX();
		int getY();
		int getW();
		int getH();
		std::string getText();
		void setTextPadded(std::string s, std::string padding = " ");
		void setTextPadded(int i, std::string padding = " ");
		unsigned int GetTextEntryLength();

	public:
		virtual void Draw(std::vector<std::vector<bool>>* pixels) override;
		void setSize(int w, int h) override;

	public:
		TextComp(mpc::Mpc& mpc, const std::string& name);
		~TextComp();

	};
}

#pragma once

#include <gui/BMFParser.hpp>
#include "TextComp.hpp"

#include <memory>
#include <cstdint>
#include <string>
#include <vector>

namespace mpc::lcdgui
{
	class Label;
}

namespace mpc::lcdgui
{

	class Field
		: public TextComp
	{

	private:
		std::vector<Label*> letters;

		bool focusable{ true };
		bool focus{ false };
		std::string csn = "";
		bool split{ false };
		int activeSplit{ 0 };
		bool typeModeEnabled{ false };
		std::string oldText = "";
		bool scrolling{ false };
		static const int BLINKING_RATE{ 200 };
		bool blinking{ false };

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
		void startBlinking();
		void stopBlinking();
		bool getBlinking();

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		Field(const std::string& name, int x, int y, int columns);
		~Field();

	};
}

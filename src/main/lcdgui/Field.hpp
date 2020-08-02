#pragma once

#include <gui/BMFParser.hpp>
#include "TextComp.hpp"

#include <memory>
#include <cstdint>
#include <string>
#include <vector>

namespace mpc { class Mpc; }

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
		mpc::Mpc& mpc;
		// When hiding a field that is focused, we pass focus to nextFocus
		std::string nextFocus = "_";

		bool focusable = true;
		bool focus = false;
		std::string csn = "";
		bool split = false;
		int activeSplit = 0;
		bool typeModeEnabled = false;
		std::string oldText = "";
		bool scrolling = false;

	public:
		bool hasFocus();
		void setFocusable(bool b);
		bool isFocusable();
		void loseFocus(std::string next);
		void takeFocus(std::string prev);
		void setSplit(bool b);
		bool isSplit();
		int getActiveSplit();
		bool setActiveSplit(int i);
		bool enableTypeMode();
		int enter();
		void type(int i);
		bool isTypeModeEnabled();
		void disableTypeMode();
		void setNextFocus(const std::string& nextFocus);

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;
		void Hide(bool b) override;

	public:
		Field(mpc::Mpc& mpc, const std::string& name, int x, int y, int width);

	};
}

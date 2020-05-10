#pragma once
#include "Component.hpp"
#include <gui/BMFParser.hpp>

#include <string>
#include <vector>
#include <memory>

namespace mpc::lcdgui
{

	class TextComp
		: public Component

	{

	protected:
		bool inverted{ false };

	protected:
		const int FONT_HEIGHT = 7;
		const int FONT_WIDTH = 6;

	protected:
		std::string text{ "" };
		int columns{ 0 }; // characters
		bool opaque{ false };

	public:
		virtual void setText(const std::string& s);

	public:
		void setOpaque(bool b);
		void setInverted(bool b);
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
		unsigned int GetTextEntryLength();

	public:
		virtual void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		TextComp(const std::string& name);
		virtual ~TextComp();

	};
}

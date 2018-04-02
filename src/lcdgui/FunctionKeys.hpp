#pragma once

#include <lcdgui/Component.hpp>

#include <gui/BMFParser.hpp>

#include <rapidjson/document.h>

#include <string>
#include <vector>

namespace mpc {
	namespace lcdgui {

		class FunctionKeys
			: public Component
		{
		private:
			std::vector<std::vector<bool>>* atlas;
			moduru::gui::bmfont* font;

		public:
			std::string name0{ "" };
			std::string name1{ "" };
			std::string name2{ "" };
			std::string name3{ "" };
			std::string name4{ "" };
			std::string name5{ "" };
			int box0{ 0 }, box1{ 0 }, box2{ 0 }, box3{ 0 }, box4{ 0 }, box5{ 0 };
			std::vector<bool> enabled;
			std::vector<std::string> names{};

		public:
			void initialize(rapidjson::Value& fbLabels, rapidjson::Value& fbTypes);
			void disable(int i);
			void enable(int i);
			void clearAll(std::vector<std::vector<bool> >* pixels);

		public:
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			FunctionKeys(std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font);
			~FunctionKeys();

		};

	}
}

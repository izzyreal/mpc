#pragma once

#include <lcdgui/Component.hpp>

#include <gui/BMFParser.hpp>

#include <rapidjson/document.h>

#include <string>
#include <vector>

namespace mpc::lcdgui
{

	class FunctionKeys
		: public Component
	{

	private:
		const std::vector<int> xPos { 2, 43, 84, 125, 166, 207 };
		int activeArrangement = 0;
		std::vector<std::vector<std::string>> texts;
		std::vector<std::vector<int>> types;
		std::vector<std::vector<bool>> enabled;

	public:
		void disable(int i);
		void enable(int i);
		void setActiveArrangement(int i);

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		FunctionKeys(const std::string& name, std::vector<std::vector<std::string>> texts, std::vector<std::vector<int>> types);
		~FunctionKeys();

	};
}
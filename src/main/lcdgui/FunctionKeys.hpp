#pragma once

#include <lcdgui/Component.hpp>

#include <gui/BMFParser.hpp>

#include <string>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::lcdgui
{
	class FunctionKey
		: public Component
	{

	public:
		FunctionKey(mpc::Mpc& mpc, const std::string& name, const int xPos);
		void Draw(std::vector<std::vector<bool>>* pixels) override;
		int type = -1;
		void setText(const std::string& text);
		void setType(const int type);
	};

	class FunctionKeys
		: public Component
	{

	private:
		const std::vector<int> xPoses { 2, 43, 84, 125, 166, 207 };
		int activeArrangement = -1;
		std::vector<std::vector<std::string>> texts;
		std::vector<std::vector<int>> types;

	public:
		void setActiveArrangement(int i);
		FunctionKeys(mpc::Mpc& mpc, const std::string& name, std::vector<std::vector<std::string>> texts, std::vector<std::vector<int>> types);

	};
}

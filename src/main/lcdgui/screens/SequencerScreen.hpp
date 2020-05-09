#pragma once

#include <lcdgui/Component.hpp>

#include <map>
#include <memory>
#include <vector>

namespace mpc::lcdgui::screens
{

	class SequencerScreen
		: public Component
	{

	private:
		std::map<std::string, std::unique_ptr<Component>> componentMap;

	public:
		void Draw(std::vector<std::vector<bool>> *pixels);

	public:
		SequencerScreen(std::map<std::string, std::unique_ptr<Component>> componentMap);

	};

}
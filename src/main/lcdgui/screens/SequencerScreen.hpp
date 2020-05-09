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
		void displaySq();

	public:
		SequencerScreen(std::vector<std::shared_ptr<Component>> componentMap);

	};

}
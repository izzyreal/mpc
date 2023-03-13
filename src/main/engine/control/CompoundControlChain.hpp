#pragma once

#include <engine/control/CompoundControl.hpp>

namespace ctoot::control
{
	class CompoundControlChain
		: public CompoundControl
	{

	public:
		void add(std::shared_ptr<Control> control) override;
		virtual void add(int index, std::shared_ptr<Control> control);

    public:
		CompoundControlChain(int id, std::string name);
	};
}

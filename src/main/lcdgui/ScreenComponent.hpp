#pragma once

#include "Component.hpp"
#include "Field.hpp"

#include <controls/BaseControls.hpp>

#include <observer/Observer.hpp>

#include <memory>
#include <string>

namespace mpc::lcdgui
{

	class ScreenComponent
		: public Component, public moduru::observer::Observer, public mpc::controls::BaseControls
	{

	protected:
		std::weak_ptr<Field> findFocus();
		void openScreen(const std::string& screenName);
		void setLastFocus(const std::string& screenName, const std::string& newLastFocus);
		const std::string getLastFocus(const std::string& screenName);

	public:
		ScreenComponent(const std::string& name);

	};

}

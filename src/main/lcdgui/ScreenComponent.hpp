#pragma once

#include "Component.hpp"
#include "Field.hpp"
#include "Label.hpp"
#include "Background.hpp"
#include "Wave.hpp"
#include "TwoDots.hpp"

#include <controls/BaseControls.hpp>

#include <observer/Observer.hpp>

#include <lang/StrUtil.hpp>

#include <memory>
#include <string>

namespace mpc::lcdgui
{

	class ScreenComponent
		: public Component, public moduru::observer::Observer, public mpc::controls::BaseControls
	{

	private:
		const int layer;

	public:
		const int& getLayerIndex();

	protected:
		std::weak_ptr<Field> findFocus();
		void openScreen(const std::string& screenName);
		void setLastFocus(const std::string& screenName, const std::string& newLastFocus);
		const std::string getLastFocus(const std::string& screenName);
		std::weak_ptr<Background> findBackground();
		std::weak_ptr<Wave> findWave();
		std::weak_ptr<TwoDots> findTwoDots();

	public:
		virtual void open() {}
		virtual void close() {}

	public:
		ScreenComponent(const std::string& name, const int layer);

	};

}

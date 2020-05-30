#pragma once

#include "Screens.hpp"
#include "Component.hpp"
#include "Field.hpp"
#include "Label.hpp"
#include "FunctionKeys.hpp"
#include "Background.hpp"
#include "Wave.hpp"
#include "TwoDots.hpp"
#include "EnvGraph.hpp"

#include <controls/BaseControls.hpp>

#include <ui/NameGui.hpp>

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
		std::weak_ptr<EnvGraph> findEnvGraph();

	public:
		virtual void open() {}
		virtual void close() {}

	public:
		ScreenComponent(const std::string& name, const int layer);

	};

}

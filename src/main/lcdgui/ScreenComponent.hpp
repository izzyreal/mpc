#pragma once
#include "Component.hpp"
#include <controls/BaseControls.hpp>
#include <observer/Observer.hpp>

#include "Screens.hpp"
#include "Field.hpp"
#include "Label.hpp"
#include "FunctionKeys.hpp"
#include "Background.hpp"
#include "Wave.hpp"
#include "TwoDots.hpp"
#include "EnvGraph.hpp"

#include <lang/StrUtil.hpp>

#include <memory>
#include <string>
#include <map>

namespace mpc::lcdgui
{
	class ScreenComponent
		: public Component, public moduru::observer::Observer, virtual public mpc::controls::BaseControls
	{

	private:
		const int layer;
		std::map<std::string, std::vector<std::string>> transferMap;
		std::string firstField = "";

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
		std::weak_ptr<Component> addChild(std::shared_ptr<Component> child) override;

	public:
		virtual void open() {}
		virtual void close() {}

	public:
		ScreenComponent(const std::string& name, const int layer);
		void setTransferMap(const std::map<std::string, std::vector<std::string>>& transferMap);
		void setFirstField(const std::string& firstField);
		std::string getFirstField();
		const std::map<std::string, std::vector<std::string>>& getTransferMap();

	};
}

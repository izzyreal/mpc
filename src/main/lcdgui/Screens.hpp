#pragma once

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace mpc::lcdgui
{
	class Component;
	class ScreenComponent;
}

namespace mpc::lcdgui
{
	
	class Screens
	{

	private:
		static std::vector<std::unique_ptr<rapidjson::Document>> Screens::layerDocuments;
		static std::map<std::string, std::shared_ptr<ScreenComponent>> screens;
		static void init();
	
	private:
		static std::vector<std::string> zoomScreenNames;

	private:
		static std::pair<std::vector<std::shared_ptr<Component>>, std::map<std::string, std::vector<std::string>>> get(const std::string& screenName, int& foundInLayer);

	public:	
		static std::shared_ptr<ScreenComponent> getScreenComponent(const std::string& screenName);

	};
}

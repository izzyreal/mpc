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
}

namespace mpc::lcdgui
{
	
	class ScreenArrangements
	{

	public:
		static std::vector<std::unique_ptr<rapidjson::Document>> ScreenArrangements::layerDocuments;
		static void init();

	public:
		static std::map<std::string, std::unique_ptr<Component>> get(const std::string& screenName, int& foundInLayer);
		static std::unique_ptr<Component> getScreenComponent(const std::string& screenName, int& foundInLayer);

	};
}

#pragma once

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <vector>
#include <string>
#include <memory>

namespace mpc::lcdgui
{
	class Component;
	class ScreenComponent;
}

namespace mpc::lcdgui
{
	
	class ScreenArrangements
	{

	public:
		static std::vector<std::unique_ptr<rapidjson::Document>> ScreenArrangements::layerDocuments;
		static void init();

	public:
		static std::vector<std::shared_ptr<Component>> get(const std::string& screenName, int& foundInLayer);
		static std::shared_ptr<ScreenComponent> getScreenComponent(const std::string& screenName, int& foundInLayer);

	};
}

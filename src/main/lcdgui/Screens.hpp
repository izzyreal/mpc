#pragma once

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace mpc {
	class Mpc;
}

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
		std::vector<std::unique_ptr<rapidjson::Document>> layerDocuments;
		std::map<std::string, std::shared_ptr<ScreenComponent>> screens;
		void init();
	
	private:
		static std::vector<std::string> zoomScreenNames;
		mpc::Mpc& mpc;

	private:
		std::pair<std::vector<std::shared_ptr<Component>>, std::map<std::string, std::vector<std::string>>> get(
			const std::string& screenName,
			int& foundInLayer,
			std::string& firstField
		);

	public:	
		std::shared_ptr<ScreenComponent> getScreenComponent(const std::string& screenName);

		template <typename T>
		std::shared_ptr<T> get(const std::string& screenName)
		{
			return dynamic_pointer_cast<T>(getScreenComponent(screenName));
		}

		Screens(mpc::Mpc& mpc);

	};
}

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

namespace mpc::lcdgui{
class Screens
{
public:
    Screens(mpc::Mpc& mpc);
    
    template <typename T>
    std::shared_ptr<T> get(const std::string& screenName)
    {
        return std::dynamic_pointer_cast<T>(getScreenComponent(screenName));
    }
    
    std::shared_ptr<ScreenComponent> getScreenComponent(const std::string& screenName);
    
private:
    static std::vector<std::string> zoomScreenNames;
    
    mpc::Mpc& mpc;
    
    void init();
    
    std::vector<std::unique_ptr<rapidjson::Document>> layerDocuments;
    std::map<std::string, std::shared_ptr<ScreenComponent>> screens;
    
    std::pair<
    std::vector<std::shared_ptr<Component>>,
    std::map<std::string, std::vector<std::string>>
    > get(const std::string& screenName,
          int& foundInLayer,
          std::string& firstField);
};
}

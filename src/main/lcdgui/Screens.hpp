#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <optional>

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
                return std::dynamic_pointer_cast<T>(getOrCreateScreenComponent(screenName));
            }

            std::shared_ptr<ScreenComponent> getOrCreateScreenComponent(const std::string& screenName);

        private:
            struct ScreenLayout
            {
                int layerIndex = -1;
                std::string firstField;
                std::vector<std::shared_ptr<Component>> components;
                std::map<std::string, std::vector<std::string>> transferMap;
            };

            static std::vector<std::string> zoomScreenNames;

            mpc::Mpc& mpc;

            std::map<std::string, std::shared_ptr<ScreenComponent>> screens;

            std::optional<ScreenLayout> getScreenLayout(const std::string& screenName);
    };
}

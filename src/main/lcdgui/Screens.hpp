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
            std::shared_ptr<T> get()
            {
                for (auto& screen : screens)
                {
                    if (auto casted = std::dynamic_pointer_cast<T>(screen))
                    {
                        return casted;
                    }
                }

                return {};
            }

            std::shared_ptr<ScreenComponent> getByName(const std::string name);

            void createAndCacheAllScreens();

        private:
            struct ScreenLayout
            {
                int layerIndex = -1;
                std::string firstField;
                std::vector<std::shared_ptr<Component>> components;
                std::map<std::string, std::vector<std::string>> transferMap;
            };

            mpc::Mpc& mpc;

            std::vector<std::shared_ptr<ScreenComponent>> screens;

            std::optional<ScreenLayout> getScreenLayout(const std::string& screenName);
            void createAndCacheScreen(const std::string &screenName);
    };
}

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <optional>

namespace mpc
{
    class Mpc;
    class AutoSave;
} // namespace mpc

namespace mpc::lcdgui
{
    class Component;
    class ScreenComponent;
} // namespace mpc::lcdgui

namespace mpc::lcdgui
{
    class Screens
    {
    public:
        Screens(mpc::Mpc &mpc);

        template <typename T>
        std::shared_ptr<T> get()
        {
            for (auto &screen : screens)
            {
                if (auto casted = std::dynamic_pointer_cast<T>(screen))
                {
                    return casted;
                }
            }

            return {};
        }

        void createAndCacheAllScreens();

        std::shared_ptr<ScreenComponent> getByName1(const std::string name);

    private:
        struct ScreenLayout
        {
            int layerIndex = -1;
            std::string firstField;
            std::vector<std::shared_ptr<Component>> components;
            std::map<std::string, std::vector<std::string>> transferMap;
        };

        mpc::Mpc &mpc;

        std::vector<std::shared_ptr<ScreenComponent>> screens;

        std::optional<ScreenLayout> getScreenLayout(const std::string &screenName);
        void createAndCacheScreen(const std::string &screenName);
    };
} // namespace mpc::lcdgui

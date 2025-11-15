#pragma once

#include "lcdgui/ScreenId.hpp"

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
        explicit Screens(Mpc &mpc);

        template <ScreenId ID>
        std::shared_ptr<typename ScreenTypeMap<ID>::type> get()
        {
            using T = typename ScreenTypeMap<ID>::type;
            for (auto &s : screens)
            {
                if (auto p = std::dynamic_pointer_cast<T>(s))
                {
                    return p;
                }
            }
            return nullptr;
        }

        std::shared_ptr<ScreenComponent> getScreenById(ScreenId screenId);

        void createAndCacheAllScreens();

    private:
        struct ScreenLayout
        {
            int layerIndex = -1;
            std::string firstField;
            std::vector<std::shared_ptr<Component>> components;
            std::map<std::string, std::vector<std::string>> transferMap;
        };

        Mpc &mpc;

        std::vector<std::shared_ptr<ScreenComponent>> screens;

        std::optional<ScreenLayout>
        getScreenLayout(const std::string &screenName) const;
        void createAndCacheScreen(const std::string &screenName);
    };
} // namespace mpc::lcdgui

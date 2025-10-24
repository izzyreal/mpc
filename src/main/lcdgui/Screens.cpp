#include "Screens.hpp"

#include "Logger.hpp"

#include "lcdgui/ScreenNames.hpp"
#include "lcdgui/AllScreens.hpp"

#include "lcdgui/Component.hpp"
#include "lcdgui/Parameter.hpp"
#include "lcdgui/Info.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/ScreenComponent.hpp"

#include <StrUtil.hpp>
#include "MpcResourceUtil.hpp"

#include <nlohmann/json.hpp>
#include <functional>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using json = nlohmann::json;

Screens::Screens(mpc::Mpc &mpc)
    : mpc(mpc)
{
}

void Screens::createAndCacheAllScreens()
{
    for (auto screenName : screenNames)
    {
        if (std::find(knownUnimplementedScreens.begin(), knownUnimplementedScreens.end(), screenName) != knownUnimplementedScreens.end())
            continue;

        createAndCacheScreen(screenName);
    }

    MLOG("Created and cached " + std::to_string(screens.size()) + " screens.");
}

std::shared_ptr<ScreenComponent> Screens::getByName1(const std::string name)
{
    for (auto &screen : screens)
    {
        if (screen->getName() == name)
            return screen;
    }

    return {};
}

std::vector<std::unique_ptr<json>> &layerDocuments()
{
    static std::vector<std::unique_ptr<json>> result;

    if (result.empty())
    {
        for (int i = 0; i < 4; i++)
        {
            const auto path = "screens/layer" + std::to_string(i + 1) + ".json";
            auto data = mpc::MpcResourceUtil::get_resource_data(path);

            auto panelDoc = std::make_unique<json>(json::parse(data.begin(), data.end()));
            result.push_back(std::move(panelDoc));
        }
    }

    return result;
}

std::vector<int> getFunctionKeyTypes(json &functionKeyTypes)
{
    std::vector<int> types;

    for (int i = 0; i < 6; i++)
    {
        if (functionKeyTypes[i].is_null())
            types.push_back(-1);
        else
            types.push_back(functionKeyTypes[i].get<int>());
    }

    return types;
}

std::vector<std::string> getFunctionKeyLabels(json &functionKeyLabels)
{
    std::vector<std::string> labels;

    for (int i = 0; i < 6; i++)
    {
        if (functionKeyLabels[i].is_null())
            labels.push_back("");
        else
            labels.push_back(functionKeyLabels[i].get<std::string>());
    }

    return labels;
}

std::optional<Screens::ScreenLayout> Screens::getScreenLayout(const std::string &screenName)
{
    Screens::ScreenLayout result;

    for (int i = 0; i < 4; i++)
    {
        if (layerDocuments()[i]->contains(screenName))
        {
            result.layerIndex = i;
            break;
        }
    }

    if (result.layerIndex < 0)
        return std::nullopt;

    json &arrangement = (*layerDocuments()[result.layerIndex])[screenName];

    if (arrangement.contains("fblabels"))
    {
        json &functionLabels = arrangement["fblabels"];
        json &functionTypes = arrangement["fbtypes"];

        std::vector<std::vector<std::string>> allLabels;
        std::vector<std::vector<int>> allTypes;

        if (!functionLabels[0].is_array())
        {
            auto labels = getFunctionKeyLabels(functionLabels);
            auto types = getFunctionKeyTypes(functionTypes);
            allLabels.push_back(labels);
            allTypes.push_back(types);
        }
        else
        {
            for (size_t i = 0; i < functionLabels.size(); i++)
            {
                auto labels = getFunctionKeyLabels(functionLabels[i]);
                auto types = getFunctionKeyTypes(functionTypes[i]);
                allLabels.push_back(labels);
                allTypes.push_back(types);
            }
        }

        auto functionKeysComponent = std::make_unique<FunctionKeys>(mpc, "function-keys", allLabels, allTypes);
        result.components.push_back(std::move(functionKeysComponent));
    }

    if (arrangement.contains("parameters"))
    {
        json &parameters = arrangement["parameters"];
        json &labels = arrangement["labels"];
        json &x = arrangement["x"];
        json &y = arrangement["y"];
        json &tfsize = arrangement["tfsize"];

        int skipCounter = 0;
        std::string previous;

        for (size_t i = 0; i < parameters.size(); i++)
        {
            if (parameters[i].is_array())
            {
                std::vector<std::string> parameterTransferMap;
                for (auto &val : parameters[i])
                    parameterTransferMap.push_back(val.get<std::string>());

                result.transferMap[previous] = parameterTransferMap;
                skipCounter++;
                continue;
            }

            auto parameterName = parameters[i].get<std::string>();
            auto parameterNames = StrUtil::split(parameterName, ',');
            std::string nextFocus = "_";

            if (parameterNames.size() > 1)
            {
                parameterName = parameterNames[0];
                nextFocus = parameterNames[1];
            }

            result.components.push_back(std::make_unique<Parameter>(
                mpc,
                labels[i - skipCounter].get<std::string>(),
                parameterName,
                x[i - skipCounter].get<int>(),
                y[i - skipCounter].get<int>(),
                tfsize[i - skipCounter].get<int>()));

            auto parameter = result.components.back();
            auto field = parameter->findField(parameterName);

            if (i == 0)
                result.firstField = parameterName;

            field->setNextFocus(nextFocus);
            previous = parameterName;
        }
    }

    if (arrangement.contains("infowidgets"))
    {
        json &infoNames = arrangement["infowidgets"];
        json &infoSize = arrangement["infosize"];
        json &infoX = arrangement["infox"];
        json &infoY = arrangement["infoy"];

        for (size_t i = 0; i < infoNames.size(); i++)
        {
            auto label = std::make_shared<Label>(mpc,
                                                 infoNames[i].get<std::string>(),
                                                 "",
                                                 infoX[i].get<int>(),
                                                 infoY[i].get<int>(),
                                                 infoSize[i].get<int>());
            result.components.push_back(label);
        }
    }

    return result;
}

using ScreenFactory = std::function<std::shared_ptr<ScreenComponent>(mpc::Mpc &, int)>;

inline const std::map<std::string, ScreenFactory> screenFactories = {
#define X(ns, Class, name) { name, [](mpc::Mpc &mpc, int layer) { return std::make_shared<mpc::lcdgui::ns::Class>(mpc, layer); } },
SCREEN_LIST
#undef X
};

static const std::map<std::string, int> screensWithoutLayoutJson{
    {"popup", 3}};

void Screens::createAndCacheScreen(const std::string &screenName)
{
    if (const auto screenFactory = screenFactories.find(screenName); screenFactory != screenFactories.end())
    {
        if (auto screenWithoutLayoutJson = screensWithoutLayoutJson.find(screenName); screenWithoutLayoutJson != screensWithoutLayoutJson.end())
        {
            auto screen = screenFactory->second(mpc, screenWithoutLayoutJson->second);
            screens.push_back(screen);
        }

        auto layout = getScreenLayout(screenName);

        if (!layout)
        {
            MLOG("Screens::getOrCreateScreenComponent has the requested screen name '" + screenName + "' in its map, and a ScreenComponent subclass for it is available in the mpc::lcdgui::screens namespace, but the layout can't be found. Most likely the layout is missing from screens/layer1.json, screens/layer2.json, screens/layer3.json or screens/layer4.json.");
            return;
        }

        auto screen = screenFactory->second(mpc, layout->layerIndex);
        screen->findBackground()->addChildren(layout->components);
        screen->setTransferMap(layout->transferMap);
        screen->setFirstField(layout->firstField);
        screens.push_back(screen);
    }

    MLOG("Screens::getOrCreateScreenComponent is not familiar with screen name '" + screenName + "'. Add it to src/main/lcdgui/Screens.cpp");
}


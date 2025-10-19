#include "Screens.hpp"

#include "Logger.hpp"

#include "lcdgui/ScreenNames.h"
#include "lcdgui/AllScreens.h"

#include <lcdgui/Component.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Info.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <lcdgui/ScreenComponent.hpp>


#include <StrUtil.hpp>

#include "MpcResourceUtil.hpp"

#include <rapidjson/document.h>

#include <functional>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace rapidjson;

Screens::Screens(mpc::Mpc &mpc)
        : mpc(mpc)
{
}

void Screens::createAndCacheAllScreens()
{
    for (auto screenName : screenNames)
    {
        if (std::find(knownUnimplementedScreens.begin(), knownUnimplementedScreens.end(), screenName) != knownUnimplementedScreens.end())
        {
            continue;
        }

        createAndCacheScreen(screenName);
    }

    MLOG("Created and cached " + std::to_string(screens.size()) + " screens.");
}

std::shared_ptr<ScreenComponent> Screens::getByName1(const std::string name)
{
    for (auto& screen : screens)
    {
        if (screen->getName() == name)
        {
            return screen;
        }
    }

    return {};
}

std::vector<std::unique_ptr<rapidjson::Document>> &layerDocuments()
{
    static std::vector<std::unique_ptr<rapidjson::Document>> result;

    if (result.empty())
    {
        for (int i = 0; i < 4; i++)
        {
            const auto path = "screens/layer" + std::to_string(i + 1) + ".json";
            auto data = mpc::MpcResourceUtil::get_resource_data(path);

            auto panelDoc = std::make_unique<Document>();
            panelDoc->Parse(&data[0], data.size());
            result.push_back(std::move(panelDoc));
        }
    }

    return result;
}

std::vector<int> getFunctionKeyTypes(Value &functionKeyTypes)
{
    std::vector<int> types;

    for (int i = 0; i < 6; i++)
    {
        if (functionKeyTypes[i].IsNull())
        {
            types.push_back(-1);
        }
        else
        {
            types.push_back(functionKeyTypes[i].GetInt());
        }
    }

    return types;
}

std::vector<std::string> getFunctionKeyLabels(Value &functionKeyLabels)
{
    std::vector<std::string> labels;

    for (int i = 0; i < 6; i++)
    {
        if (functionKeyLabels[i].IsNull())
        {
            labels.push_back("");
        }
        else
        {
            labels.push_back(functionKeyLabels[i].GetString());
        }
    }

    return labels;
}

std::optional<Screens::ScreenLayout> Screens::getScreenLayout(const std::string& screenName)
{
    Screens::ScreenLayout result;

    for (int i = 0; i < 4; i++)
    {
        if (layerDocuments()[i]->HasMember(screenName.c_str()))
        {
            result.layerIndex = i;
            break;
        }
    }

    if (result.layerIndex < 0)
    {
        return std::nullopt;
    }

    Value &arrangement = layerDocuments()[result.layerIndex]->GetObj()[screenName.c_str()];

    if (arrangement.HasMember("fblabels"))
    {
        Value &functionLabels = arrangement["fblabels"];
        Value &functionTypes = arrangement["fbtypes"];

        std::vector<std::vector<std::string>> allLabels;
        std::vector<std::vector<int>> allTypes;

        if (!functionLabels[0].IsArray())
        {
            auto labels = getFunctionKeyLabels(functionLabels);
            auto types = getFunctionKeyTypes(functionTypes);
            allLabels.push_back(labels);
            allTypes.push_back(types);
        }
        else
        {
            for (int i = 0; i < functionLabels.Size(); i++)
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

    if (arrangement.HasMember("parameters"))
    {
        Value &parameters = arrangement["parameters"];
        Value &labels = arrangement["labels"];
        Value &x = arrangement["x"];
        Value &y = arrangement["y"];
        Value &tfsize = arrangement["tfsize"];

        std::vector<std::string> row;

        int skipCounter = 0;

        std::string previous;

        for (int i = 0; i < parameters.Size(); i++)
        {
            if (parameters[i].IsArray())
            {
                std::vector<std::string> parameterTransferMap;

                for (int j = 0; j < parameters[i].Size(); j++)
                {
                    parameterTransferMap.push_back(parameters[i][j].GetString());
                }

                result.transferMap[previous] = parameterTransferMap;
                skipCounter++;
                continue;
            }

            auto parameterName = std::string(parameters[i].GetString());
            auto parameterNames = StrUtil::split(parameterName, ',');
            std::string nextFocus = "_";

            if (parameterNames.size() > 1)
            {
                parameterName = parameterNames[0];
                nextFocus = parameterNames[1];
            }

            row.push_back(parameterName);

            result.components.push_back(std::make_unique<Parameter>(
                    mpc,
                    labels[i - skipCounter].GetString(),
                    parameterName,
                    x[i - skipCounter].GetInt(),
                    y[i - skipCounter].GetInt(),
                    tfsize[i - skipCounter].GetInt()));

            auto parameter = result.components.back();
            auto field = parameter->findField(parameterName);

            if (i == 0)
            {
                result.firstField = parameterName;
            }

            field->setNextFocus(nextFocus);
            previous = parameterName;
        }
    }

    if (arrangement.HasMember("infowidgets"))
    {
        Value &infoNames = arrangement["infowidgets"];
        Value &infoSize = arrangement["infosize"];
        Value &infoX = arrangement["infox"];
        Value &infoY = arrangement["infoy"];

        for (int i = 0; i < infoNames.Size(); i++)
        {
            auto label = std::make_shared<Label>(mpc,
                                                 infoNames[i].GetString(),
                                                 "",
                                                 infoX[i].GetInt(),
                                                 infoY[i].GetInt(),
                                                 infoSize[i].GetInt());
            result.components.push_back(label);
        }
    }

    return result;
}

using ScreenFactory = std::function<std::shared_ptr<ScreenComponent>(mpc::Mpc&, int)>;

inline const std::map<std::string, ScreenFactory> screenFactories = {
#define X(ns, Class, name) { name, [](mpc::Mpc& mpc, int layer){ return std::make_shared<mpc::lcdgui::ns::Class>(mpc, layer); } },
SCREEN_LIST
#undef X
};

// screenName -> layerIndex
static const std::map<std::string, int> screensWithoutLayoutJson {
    { "popup", 3 }
};

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


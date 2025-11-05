#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <deque>

#include "lcdgui/ScreenId.hpp"
#include "lcdgui/BasicStructs.hpp"
#include "lcdgui/BMFStructs.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{
    class FunctionKeys;
    class Background;
    class Field;
    class Label;
    class Layer;
    class Component;
    class ScreenComponent;
} // namespace mpc::lcdgui

namespace mpc::lcdgui
{

    class LayeredScreen
    {

    private:
        mpc::Mpc &mpc;
        std::unique_ptr<Component> root;
        std::vector<std::vector<bool>> pixels =
            std::vector<std::vector<bool>>(248, std::vector<bool>(60));
        std::deque<std::shared_ptr<ScreenComponent>> history;

    public:
        std::vector<std::vector<bool>> atlas;
        bmfont font;

    private:
        const int LAYER_COUNT = 4;

        std::vector<std::shared_ptr<Layer>> layers;

        // 0 = left, 1 = right, 2 = up, 3 = down
        bool transfer(int direction);

    public:
        void transferLeft();
        void transferRight();
        void transferUp();
        void transferDown();

        void showPopupForMs(const std::string msg, const int delayMs);

        void showPopupAndThenReturnToLayer(const std::string msg,
                                           const int delayMs,
                                           const int layerIndex);

        void showPopupAndAwaitInteraction(const std::string msg);

        int getFocusedLayerIndex();
        std::shared_ptr<Layer> getFocusedLayer();

        void openScreenById(const ScreenId);

        bool isPreviousScreen(std::initializer_list<ScreenId> ids) const;

        bool isPreviousScreenNot(std::initializer_list<ScreenId> ids) const;

        bool isCurrentScreen(std::initializer_list<ScreenId> ids) const;

        void showPopupAndThenOpen(ScreenId targetId, const std::string &msg,
                                  int delayMs);

        bool isCurrentScreenPopupFor(ScreenId targetId) const;

        void openScreen(const std::string screenName);

        std::shared_ptr<ScreenComponent> getCurrentScreen();

        void closeCurrentScreen();

        void closeRecentScreensUntilReachingLayer(const int layerIndex);

    private:
        void setPopupScreenText(const std::string);
        std::map<std::string, std::string> lastFocuses;

        void openScreenInternal(std::shared_ptr<ScreenComponent>);
        void returnToLastFocus(std::shared_ptr<ScreenComponent> screen,
                               std::string firstFieldOfThisScreen);

        static ScreenId
        getScreenId(const std::shared_ptr<ScreenComponent> &screen);

    private:
        FunctionKeys *getFunctionKeys();

    public:
        lcdgui::Background *getCurrentBackground();
        void setCurrentBackground(std::string s);

    public:
        void setLastFocus(std::string screenName, std::string tfName);
        std::string getLastFocus(std::string screenName);

        std::string getCurrentScreenName();
        std::string getFirstLayerScreenName();
        void setFunctionKeysArrangement(int arrangementIndex);

    public:
        std::vector<std::vector<bool>> *getPixels();
        bool IsDirty();
        void setDirty();
        MRECT getDirtyArea();
        void Draw();
        void timerCallback();

    public:
        std::string getFocusedFieldName();
        std::shared_ptr<Field> getFocusedField();
        bool setFocus(const std::string &focus);

    public:
        LayeredScreen(mpc::Mpc &mpc);
    };
} // namespace mpc::lcdgui

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "BMFParser.hpp"

#include "BasicStructs.hpp"

namespace mpc { class Mpc; }

namespace mpc::lcdgui
{
	class FunctionKeys;
	class Background;
	class Field;
	class Label;
	class Layer;
	class Component;
	class ScreenComponent;
}

namespace mpc::lcdgui {

	class LayeredScreen
	{

	private:
		mpc::Mpc& mpc;
		std::unique_ptr<Component> root;
		std::vector<std::vector<bool>> pixels = std::vector<std::vector<bool>>(248, std::vector<bool>(60));

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
		int getFocusedLayerIndex();
		std::shared_ptr<Layer> getFocusedLayer();
		void openScreen(std::string newScreenName);
		std::shared_ptr<ScreenComponent> findScreenComponent();

	private:
		std::map<std::string, std::string> lastFocuses;
		int focusedLayerIndex{ 0 };
		std::string currentScreenName;
		std::string previousScreenName;
        std::string screenToReturnToWhenPressingOpenWindow;

	private:
		FunctionKeys* getFunctionKeys();

	public:
		lcdgui::Background* getCurrentBackground();
		void setCurrentBackground(std::string s);

	public:
		void returnToLastFocus(std::string firstFieldOfThisScreen);
		void setLastFocus(std::string screenName, std::string tfName);
		std::string getLastFocus(std::string screenName);

        std::string getCurrentScreenName();
		void setPreviousScreenName(std::string screenName);
		std::string getPreviousScreenName();
		void setFunctionKeysArrangement(int arrangementIndex);

        std::string getScreenToReturnToWhenPressingOpenWindow();
        void setScreenToReturnToWhenPressingOpenWindow(const std::string);
        void clearScreenToReturnToWhenPressingOpenWindow();

	public:
		std::vector<std::vector<bool>>* getPixels();
		bool IsDirty();
		void setDirty();
		MRECT getDirtyArea();
		void Draw();

	public:
		std::string getFocus();
        std::shared_ptr<Field> getFocusedField();
		bool setFocus(const std::string& focus);

	public:
		LayeredScreen(mpc::Mpc& mpc);

	};
}

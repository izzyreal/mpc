#pragma once

#include <memory>
#include <vector>
#include <string>

#include <gui/BMFParser.hpp>

#include <gui/BasicStructs.hpp>

namespace mpc::lcdgui
{
	class FunctionKeys;
	class Background;
	class Popup;
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
		std::unique_ptr<Component> root;
		std::vector<std::vector<bool>> pixels = std::vector<std::vector<bool>>(248, std::vector<bool>(60));

	public:
		static std::vector<std::vector<bool>> atlas;
		static moduru::gui::bmfont font;

	private:
		const int LAYER_COUNT = 4;

		std::vector<std::weak_ptr<Layer>> layers;

		int getCurrentFieldIndex();
		std::weak_ptr<mpc::lcdgui::Field> findBelow(std::weak_ptr<mpc::lcdgui::Field> tf);
		std::weak_ptr<mpc::lcdgui::Field> findAbove(std::weak_ptr<mpc::lcdgui::Field> tf);

	public:
		void transferLeft();
		void transferRight();
		std::string findBelow(std::string tf);
		std::string findAbove(std::string tf);
		//void transferFocus(bool backwards);
		Layer* getLayer(int i);
		int getFocusedLayerIndex();
		std::weak_ptr<Layer> getFocusedLayer();
		int openScreen(std::string name); // returns layer number
		std::weak_ptr<ScreenComponent> findScreenComponent();

	private:
		std::vector<std::vector<std::string>> lastFocuses{};
		int focusedLayerIndex{ 0 };
		std::string currentScreenName = "";
		std::string previousScreenName = "";
		std::shared_ptr<Popup> popup;
		int previousLayer{ 0 };
		std::string previousViewModeText = "";
		std::string previousFromNoteText = "";

	private:
		FunctionKeys* getFunctionKeys();

	public:
		void createPopup(std::string text, int textXPos);
		void openFileNamePopup(const std::string& name, const std::string& ext);
		lcdgui::Background* getCurrentBackground();
		void removeCurrentBackground();
		void setCurrentBackground(std::string s);
		void removePopup();
		void setPopupText(std::string text);

	public:
		void returnToLastFocus(std::string firstFieldOfThisScreen);
		void setLastFocus(std::string screenName, std::string tfName);
		std::string getLastFocus(std::string screenName);
		void setCurrentScreenName(std::string screenName);
		std::string getCurrentScreenName();
		void setPreviousScreenName(std::string screenName);
		std::string getPreviousScreenName();
		mpc::lcdgui::Popup* getPopup();
		std::string getPreviousFromNoteText();
		void setPreviousFromNoteText(std::string text);
		void setPreviousViewModeText(std::string text);
		std::string getPreviousViewModeText();
		void setFunctionKeysArrangement(int arrangementIndex);

	public:
		std::vector<std::vector<bool>>* getPixels();
		bool IsDirty();
		MRECT getDirtyArea();
		void Draw();

	public:
		std::string getFocus();
		void setFocus(const std::string& focus);

	public:
		LayeredScreen();

	};
}

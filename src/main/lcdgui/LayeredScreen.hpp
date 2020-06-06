#pragma once

#include <memory>
#include <vector>
#include <string>

#include <gui/BMFParser.hpp>

#include <gui/BasicStructs.hpp>

namespace moduru::observer {
	class Observer;
}

namespace mpc::lcdgui {
	class FunctionKeys;
	class SelectedEventBar;
	class Background;
	class EnvGraph;
	class TwoDots;
	class HorizontalBar;
	class Underline;
	class Popup;
	class Wave;
	class Effect;
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
		std::string findBelow(std::string tf);
		std::string findAbove(std::string tf);
		void transferFocus(bool backwards);
		Layer* getLayer(int i);
		int getFocusedLayerIndex();
		std::weak_ptr<Layer> getFocusedLayer();
		int openScreen(std::string name); // returns layer number
		std::weak_ptr<Field> lookupField(std::string s);
		std::weak_ptr<Label> lookupLabel(std::string s);
		std::weak_ptr<ScreenComponent> findScreenComponent();

	private:
		std::vector<std::vector<std::string>> lastFocuses{};

		int focusedLayerIndex{ 0 };
		std::string currentScreenName = "";
		std::string previousScreenName = "";

	private:
		std::vector<std::weak_ptr<mpc::lcdgui::Component>> nonTextComps;

	private:
		std::shared_ptr<Popup> popup;
		std::shared_ptr<EnvGraph> envGraph;
		std::shared_ptr<TwoDots> twoDots;
		std::vector<std::shared_ptr<HorizontalBar>> horizontalBarsStepEditor;
		std::vector<std::shared_ptr<SelectedEventBar>> selectedEventBarsStepEditor;
		std::vector<std::shared_ptr<HorizontalBar>> horizontalBarsTempoChangeEditor;
		std::shared_ptr<Underline> underline;
		std::shared_ptr<Wave> fineWave;
		std::shared_ptr<Wave> wave;
		std::vector<std::shared_ptr<Effect>> effects;

	private:
		int previousLayer{ 0 };
		std::string previousViewModeText = "";
		std::string previousFromNoteText = "";

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
		std::vector<std::weak_ptr<HorizontalBar>> getHorizontalBarsTempoChangeEditor();
		std::vector<std::weak_ptr<HorizontalBar>>getHorizontalBarsStepEditor();
		std::vector<std::weak_ptr<SelectedEventBar>> getSelectedEventBarsStepEditor();
		FunctionKeys* getFunctionKeys();
		void setFunctionKeysArrangement(int arrangementIndex);
		std::weak_ptr<Underline> getUnderline();
		std::weak_ptr<TwoDots> getTwoDots();
		std::weak_ptr<Wave> getFineWave();
		std::weak_ptr<Wave> getWave();
		std::weak_ptr<EnvGraph> getEnvGraph();
		std::vector<std::weak_ptr<Effect>> getEffects();

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

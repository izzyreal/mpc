#pragma once
#include <lcdgui/Layer.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <memory>
#include <vector>
#include <string>

#include <gui/BMFParser.hpp>

namespace moduru {
	namespace observer {
		class Observer;
	}
}

namespace mpc {

	class Mpc;

	namespace lcdgui {
		class SelectedEventBar;
		class Background;
		class EnvGraph;
		class TwoDots;
		class HorizontalBar;
		class Underline;
		class VerticalBar;
		class MixerFaderBackground;
		class MixerTopBackground;
		class Knob;
		class Popup;
		class Wave;
		class Effect;
		class Field;
		class Label;


		class LayeredScreen
		{

		private:
			MRECT dirtyArea;
			std::vector<std::vector<bool> > pixels;
			std::unique_ptr<moduru::observer::Observer> activeObserver;
			std::vector<std::vector<bool>> atlas;
			moduru::gui::bmfont font;

		private:
			void initObserver();

		private:
			static const int LAYER_COUNT{ 4 };

			mpc::Mpc* mpc;

			std::vector<rapidjson::Document> layerJsons = std::vector<rapidjson::Document>(LAYER_COUNT);
			std::vector<std::unique_ptr<Layer>> layers = std::vector<std::unique_ptr<Layer>>(LAYER_COUNT);

			int getCurrentParamIndex();
			std::weak_ptr<mpc::lcdgui::Field> findBelow(std::weak_ptr<mpc::lcdgui::Field> tf);
			std::weak_ptr<mpc::lcdgui::Field> findAbove(std::weak_ptr<mpc::lcdgui::Field> tf);

		public:
			std::string findBelow(std::string tf);
			std::string findAbove(std::string tf);
			void transferFocus(bool backwards);
			Layer* getLayer(int i);
			int getCurrentLayer();
			int openScreen(std::string name); // returns layer number
			std::weak_ptr<Field> lookupField(std::string s);
			std::weak_ptr<Label> lookupLabel(std::string s);

		private:
			std::vector<std::vector<std::string>> lastFocus{};

			int currentLayer{ 0 };
			std::string currentScreenName{ "" };
			std::string previousScreenName{ "" };

		private:
			std::vector<std::weak_ptr<mpc::lcdgui::Component> > nonTextComps;

		private:
			std::shared_ptr<Popup> popup;
			std::shared_ptr<EnvGraph> envGraph;
			std::shared_ptr<TwoDots> twoDots;
			std::vector<std::shared_ptr<HorizontalBar>> horizontalBarsStepEditor;
			std::vector<std::shared_ptr<SelectedEventBar>> selectedEventBarsStepEditor;
			std::vector<std::shared_ptr<HorizontalBar>> horizontalBarsTempoChangeEditor;
			std::shared_ptr<Underline> underline;
			std::vector<std::shared_ptr<VerticalBar>> verticalBarsMixer;
			std::vector<std::shared_ptr<MixerFaderBackground>> mixerFaderBackgrounds;
			std::vector<std::shared_ptr<MixerTopBackground>> mixerTopBackgrounds;
			std::vector<std::shared_ptr<Knob>> knobs;
			std::shared_ptr<Wave> fineWave;
			std::shared_ptr<Wave> wave;
			std::vector<std::shared_ptr<Effect>> effects;

		private:
			int previousLayer{ 0 };
			std::string previousViewModeText{ "" };
			std::string previousFromNoteText{ "" };

		public:
			void createPopup(std::string text, int textXPos);
			lcdgui::Background* getCurrentBackground();
			void removeCurrentBackground();
			void setCurrentBackground(std::string s);
			void removePopup();
			void setPopupText(std::string text);

		public:
			void returnToLastFocus(std::string firstFieldOfThisScreen);
			void redrawEnvGraph(int attack, int decay);
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
			std::vector<std::weak_ptr<VerticalBar>> getVerticalBarsMixer();
			std::vector<std::weak_ptr<SelectedEventBar>> getSelectedEventBarsStepEditor();
			FunctionKeys* getFunctionKeys();
			std::vector<std::weak_ptr<Knob>> getKnobs();
			std::vector<std::weak_ptr<MixerFaderBackground>> getMixerFaderBackgrounds();
			std::vector<std::weak_ptr<MixerTopBackground>> getMixerTopBackgrounds();
			void drawFunctionKeys(std::string screenName);
			std::weak_ptr<Underline> getUnderline();
			std::weak_ptr<TwoDots> getTwoDots();
			std::weak_ptr<Wave> getFineWave();
			std::weak_ptr<Wave> getWave();
			std::weak_ptr<EnvGraph> getEnvGraph();
			std::vector<std::weak_ptr<Effect>> getEffects();

		public:
			std::vector<std::vector<bool> >* getPixels();
			bool IsDirty();
			MRECT* getDirtyArea();
			void Draw();

		public:
			std::string getFocus();
			void setFocus(std::string focus);
			//void setFocus(std::string focus, int layer);

		public:
			LayeredScreen(mpc::Mpc* mpc);
			~LayeredScreen();

		};
	}
}

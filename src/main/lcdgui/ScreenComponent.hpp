#pragma once
#include "Component.hpp"
#include <observer/Observer.hpp>

#include <controls/BaseControls.hpp>

#include "Screens.hpp"
#include "Field.hpp"
#include "Label.hpp"
#include "FunctionKeys.hpp"
#include "Background.hpp"
#include "Wave.hpp"
#include "EnvGraph.hpp"

#include <lang/StrUtil.hpp>

#include <memory>
#include <string>
#include <map>

#include "engine/Drum.hpp"

namespace mpc::lcdgui
{
	class ScreenComponent
		: public Component, public moduru::observer::Observer
	{

	private:
		const int layer;
		std::map<std::string, std::vector<std::string>> transferMap;
		std::string firstField = "";

	public:
		const int& getLayerIndex();
		
	protected:
		mpc::Mpc& mpc;
		std::shared_ptr<mpc::sampler::Sampler> sampler;
		std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
		std::shared_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::string param = "";
		std::shared_ptr<mpc::sampler::Program> program;
		std::shared_ptr<mpc::sequencer::Track> track;
		mpc::engine::Drum& mpcSoundPlayerChannel() { return mpc.getControls()->getControls()->mpcSoundPlayerChannel; }

	protected:
		std::shared_ptr<Field> findFocus();
		void openScreen(const std::string& screenName);
		void setLastFocus(const std::string& screenName, const std::string& newLastFocus);
		const std::string getLastFocus(const std::string& screenName);
		std::shared_ptr<Wave> findWave();
		std::shared_ptr<EnvGraph> findEnvGraph();

	public:
		virtual void open() {}
		virtual void close() {}

	public:
		ScreenComponent(mpc::Mpc& mpc, const std::string& name, const int layer);
		void setTransferMap(const std::map<std::string, std::vector<std::string>>& newTransferMap);
		void setFirstField(const std::string& newFirstField);
		std::string getFirstField();
		std::map<std::string, std::vector<std::string>>& getTransferMap();

	protected:
		virtual void init() {
	
			auto controls = mpc.getControls()->getControls();
			
			// Happens when loading NVRAM
			if (!controls)
				return;

			controls->init();
			param = controls->param;
			program = controls->program;
			track = controls->track;
		}

	public:
		void splitLeft() { mpc.getControls()->getControls()->splitLeft(); }
		void splitRight() { mpc.getControls()->getControls()->splitRight(); }
		virtual void left() { mpc.getControls()->getControls()->left(); }
		virtual void right() { mpc.getControls()->getControls()->right(); }
		virtual void up() { mpc.getControls()->getControls()->up(); }
		virtual void down() { mpc.getControls()->getControls()->down(); }
		virtual void function(int i) { init(); mpc.getControls()->getControls()->function(i); }
		virtual void openWindow() {}
		virtual void turnWheel(int i) {}
		virtual void numpad(int i) { mpc.getControls()->getControls()->numpad(i); }
		virtual void pressEnter() { mpc.getControls()->getControls()->pressEnter(); }
		virtual void rec() { mpc.getControls()->getControls()->rec(); }
		virtual void overDub() { mpc.getControls()->getControls()->overDub(); }
		virtual void stop() { mpc.getControls()->getControls()->stop(); }
		virtual void play() { mpc.getControls()->getControls()->play(); }
		virtual void playStart() { mpc.getControls()->getControls()->playStart(); }
		virtual void mainScreen() { mpc.getControls()->getControls()->mainScreen(); }
		virtual void tap() { mpc.getControls()->getControls()->tap(); }
		virtual void prevStepEvent() { mpc.getControls()->getControls()->prevStepEvent(); }
		virtual void nextStepEvent() { mpc.getControls()->getControls()->nextStepEvent(); }
		virtual void goTo() { mpc.getControls()->getControls()->goTo(); }
		virtual void prevBarStart() { mpc.getControls()->getControls()->prevBarStart(); }
		virtual void nextBarEnd() { mpc.getControls()->getControls()->nextBarEnd(); }
		virtual void nextSeq() { mpc.getControls()->getControls()->nextSeq(); }
		virtual void trackMute() { mpc.getControls()->getControls()->trackMute(); }
		virtual void bank(int i) { mpc.getControls()->getControls()->bank(i); }
		virtual void fullLevel() { mpc.getControls()->getControls()->fullLevel(); }
		virtual void sixteenLevels() { mpc.getControls()->getControls()->sixteenLevels(); }
		virtual void after() { mpc.getControls()->getControls()->after(); }
		virtual void shift() { mpc.getControls()->getControls()->shift(); }
		virtual void undoSeq() { mpc.getControls()->getControls()->undoSeq(); }
		virtual void erase() { mpc.getControls()->getControls()->erase(); }
		virtual void setSlider(int i) { mpc.getControls()->getControls()->setSlider(i); }

		virtual bool isTypable() { return mpc.getControls()->getControls()->isTypable(); }

		int getSoundIncrement(int notch) { return mpc.getControls()->getControls()->getSoundIncrement(notch); }

		virtual void pad(int padIndexWithBank, int velo) { mpc.getControls()->getControls()->pad(padIndexWithBank, velo); }

	};
}

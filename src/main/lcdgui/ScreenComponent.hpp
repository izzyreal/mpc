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
		mpc::engine::Drum& mpcSoundPlayerChannel() { return mpc.getControls()->getBaseControls()->mpcSoundPlayerChannel; }

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
	
			auto controls = mpc.getControls()->getBaseControls();
			
			// Happens when loading NVRAM
			if (!controls)
				return;

			controls->init();
			param = controls->param;
			program = controls->program;
			track = controls->track;
		}

	public:
		void splitLeft() { mpc.getControls()->getBaseControls()->splitLeft(); }
		void splitRight() { mpc.getControls()->getBaseControls()->splitRight(); }
		virtual void left() { mpc.getControls()->getBaseControls()->left(); }
		virtual void right() { mpc.getControls()->getBaseControls()->right(); }
		virtual void up() { mpc.getControls()->getBaseControls()->up(); }
		virtual void down() { mpc.getControls()->getBaseControls()->down(); }
		virtual void function(int i) { init(); mpc.getControls()->getBaseControls()->function(i); }
		virtual void openWindow() {}
		virtual void turnWheel(int i) {}
		virtual void numpad(int i) { mpc.getControls()->getBaseControls()->numpad(i); }
		virtual void pressEnter() { mpc.getControls()->getBaseControls()->pressEnter(); }
		virtual void rec() { mpc.getControls()->getBaseControls()->rec(); }
		virtual void overDub() { mpc.getControls()->getBaseControls()->overDub(); }
		virtual void stop() { mpc.getControls()->getBaseControls()->stop(); }
		virtual void play() { mpc.getControls()->getBaseControls()->play(); }
		virtual void playStart() { mpc.getControls()->getBaseControls()->playStart(); }
		virtual void mainScreen() { mpc.getControls()->getBaseControls()->mainScreen(); }
		virtual void tap() { mpc.getControls()->getBaseControls()->tap(); }
		virtual void prevStepEvent() { mpc.getControls()->getBaseControls()->prevStepEvent(); }
		virtual void nextStepEvent() { mpc.getControls()->getBaseControls()->nextStepEvent(); }
		virtual void goTo() { mpc.getControls()->getBaseControls()->goTo(); }
		virtual void prevBarStart() { mpc.getControls()->getBaseControls()->prevBarStart(); }
		virtual void nextBarEnd() { mpc.getControls()->getBaseControls()->nextBarEnd(); }
		virtual void nextSeq() { mpc.getControls()->getBaseControls()->nextSeq(); }
		virtual void trackMute() { mpc.getControls()->getBaseControls()->trackMute(); }
		virtual void bank(int i) { mpc.getControls()->getBaseControls()->bank(i); }
		virtual void fullLevel() { mpc.getControls()->getBaseControls()->fullLevel(); }
		virtual void sixteenLevels() { mpc.getControls()->getBaseControls()->sixteenLevels(); }
		virtual void after() { mpc.getControls()->getBaseControls()->after(); }
		virtual void shift() { mpc.getControls()->getBaseControls()->shift(); }
		virtual void undoSeq() { mpc.getControls()->getBaseControls()->undoSeq(); }
		virtual void erase() { mpc.getControls()->getBaseControls()->erase(); }
		virtual void setSlider(int i) { mpc.getControls()->getBaseControls()->setSlider(i); }

		virtual bool isTypable() { return mpc.getControls()->getBaseControls()->isTypable(); }

		int getSoundIncrement(int notch) { return mpc.getControls()->getBaseControls()->getSoundIncrement(notch); }

		virtual void pad(int padIndexWithBank, int velo) { mpc.getControls()->getBaseControls()->pad(padIndexWithBank, velo); }

	};
}

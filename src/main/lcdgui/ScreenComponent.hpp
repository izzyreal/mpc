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
		std::weak_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::string param = "";
		std::weak_ptr<mpc::sampler::Program> program;
		std::weak_ptr<mpc::sequencer::Track> track;
		ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel = nullptr;

	protected:
		std::weak_ptr<Field> findFocus();
		void openScreen(const std::string& screenName);
		void setLastFocus(const std::string& screenName, const std::string& newLastFocus);
		const std::string getLastFocus(const std::string& screenName);
		std::weak_ptr<Wave> findWave();
		std::weak_ptr<EnvGraph> findEnvGraph();

		template<typename T, typename... Args>
		std::weak_ptr<T> addChildT(Args... args)
		{
			auto child = std::make_shared<T>(args...);
			addChild(child);
			return child;
		}

	public:
		virtual void open() {}
		virtual void close() {}

	public:
		ScreenComponent(mpc::Mpc& mpc, const std::string& name, const int layer);
		void setTransferMap(const std::map<std::string, std::vector<std::string>>& newTransferMap);
		void setFirstField(const std::string& newFirstField);
		std::string getFirstField();
		const std::map<std::string, std::vector<std::string>>& getTransferMap();

	protected:		
		virtual void init() {
	
			auto controls = mpc.getControls().lock()->getControls();
			
			// Happens when loading NVRAM
			if (!controls)
				return;

			controls->init();
			param = controls->param;
			program = controls->program;
			mpcSoundPlayerChannel = controls->mpcSoundPlayerChannel;
			track = controls->track;
		}

	public:
		void splitLeft() { mpc.getControls().lock()->getControls()->splitLeft(); }
		void splitRight() { mpc.getControls().lock()->getControls()->splitRight(); }
		virtual void left() { mpc.getControls().lock()->getControls()->left(); }
		virtual void right() { mpc.getControls().lock()->getControls()->right(); }
		virtual void up() { mpc.getControls().lock()->getControls()->up(); }
		virtual void down() { mpc.getControls().lock()->getControls()->down(); }
		virtual void function(int i) { init(); mpc.getControls().lock()->getControls()->function(i); }
		virtual void openWindow() {}
		virtual void turnWheel(int i) {}
		virtual void numpad(int i) { mpc.getControls().lock()->getControls()->numpad(i); }
		virtual void pressEnter() { mpc.getControls().lock()->getControls()->pressEnter(); }
		virtual void rec() { mpc.getControls().lock()->getControls()->rec(); }
		virtual void overDub() { mpc.getControls().lock()->getControls()->overDub(); }
		virtual void stop() { mpc.getControls().lock()->getControls()->stop(); }
		virtual void play() { mpc.getControls().lock()->getControls()->play(); }
		virtual void playStart() { mpc.getControls().lock()->getControls()->playStart(); }
		virtual void mainScreen() { mpc.getControls().lock()->getControls()->mainScreen(); }
		virtual void tap() { mpc.getControls().lock()->getControls()->tap(); }
		virtual void prevStepEvent() { mpc.getControls().lock()->getControls()->prevStepEvent(); }
		virtual void nextStepEvent() { mpc.getControls().lock()->getControls()->nextStepEvent(); }
		virtual void goTo() { mpc.getControls().lock()->getControls()->goTo(); }
		virtual void prevBarStart() { mpc.getControls().lock()->getControls()->prevBarStart(); }
		virtual void nextBarEnd() { mpc.getControls().lock()->getControls()->nextBarEnd(); }
		virtual void nextSeq() { mpc.getControls().lock()->getControls()->nextSeq(); }
		virtual void trackMute() { mpc.getControls().lock()->getControls()->trackMute(); }
		virtual void bank(int i) { mpc.getControls().lock()->getControls()->bank(i); }
		virtual void fullLevel() { mpc.getControls().lock()->getControls()->fullLevel(); }
		virtual void sixteenLevels() { mpc.getControls().lock()->getControls()->sixteenLevels(); }
		virtual void after() { mpc.getControls().lock()->getControls()->after(); }
		virtual void shift() { mpc.getControls().lock()->getControls()->shift(); }
		virtual void undoSeq() { mpc.getControls().lock()->getControls()->undoSeq(); }
		virtual void erase() { mpc.getControls().lock()->getControls()->erase(); }
		virtual void setSlider(int i) { mpc.getControls().lock()->getControls()->setSlider(i); }

		virtual bool isTypable() { return mpc.getControls().lock()->getControls()->isTypable(); }

		int getSoundIncrement(int notch) { return mpc.getControls().lock()->getControls()->getSoundIncrement(notch); }

		virtual void pad(int padIndexWithBank, int velo, bool repeat, int tick) { mpc.getControls().lock()->getControls()->pad(padIndexWithBank, velo, repeat, tick); }

	};
}

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
		mpc::controls::BaseControls* getBaseControls() { if (baseControls) return baseControls.get(); return nullptr; }
		
	protected:
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::weak_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::string param = "";
		std::weak_ptr<mpc::sampler::Program> program;
		std::weak_ptr<mpc::sequencer::Track> track;
		ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel = nullptr;

	protected:
		std::shared_ptr<mpc::controls::BaseControls> baseControls;
		std::weak_ptr<Field> findFocus();
		void openScreen(const std::string& screenName);
		void setLastFocus(const std::string& screenName, const std::string& newLastFocus);
		const std::string getLastFocus(const std::string& screenName);
		std::weak_ptr<Background> findBackground();
		std::weak_ptr<Wave> findWave();
		std::weak_ptr<EnvGraph> findEnvGraph();
		std::weak_ptr<Component> addChild(std::shared_ptr<Component> child) override;

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
		void setTransferMap(const std::map<std::string, std::vector<std::string>>& transferMap);
		void setFirstField(const std::string& firstField);
		std::string getFirstField();
		const std::map<std::string, std::vector<std::string>>& getTransferMap();

	protected:
		std::vector<int> splitInc{ 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1 };
		virtual void init() { baseControls->init(); param = baseControls->param; program = baseControls->program; mpcSoundPlayerChannel = baseControls->mpcSoundPlayerChannel; track = baseControls->track; }

	public:
		void splitLeft() { baseControls->splitLeft(); }
		void splitRight() { baseControls->splitRight(); }
		virtual void left() { baseControls->left(); }
		virtual void right() { baseControls->right(); }
		virtual void up() { baseControls->up(); }
		virtual void down() { baseControls->down(); }
		virtual void function(int i) { baseControls->function(i); }
		virtual void openWindow() { baseControls->openWindow(); }
		virtual void turnWheel(int i) { baseControls->turnWheel(i); }
		virtual void numpad(int i) { baseControls->numpad(i); }
		virtual void pressEnter() { baseControls->pressEnter(); }
		virtual void rec() { baseControls->rec(); }
		virtual void overDub() { baseControls->overDub(); }
		virtual void stop() { baseControls->stop(); }
		virtual void play() { baseControls->play(); }
		virtual void playStart() { baseControls->playStart(); }
		virtual void mainScreen() { baseControls->mainScreen(); }
		virtual void tap() { baseControls->tap(); }
		virtual void prevStepEvent() { baseControls->prevStepEvent(); }
		virtual void nextStepEvent() { baseControls->nextStepEvent(); }
		virtual void goTo() { baseControls->goTo(); }
		virtual void prevBarStart() { baseControls->prevBarStart(); }
		virtual void nextBarEnd() { baseControls->nextBarEnd(); }
		virtual void nextSeq() { baseControls->nextSeq(); }
		virtual void trackMute() { baseControls->trackMute(); }
		virtual void bank(int i) { baseControls->bank(i); }
		virtual void fullLevel() { baseControls->fullLevel(); }
		virtual void sixteenLevels() { baseControls->sixteenLevels(); }
		virtual void after() { baseControls->after(); }
		virtual void shift() { baseControls->shift(); }
		virtual void undoSeq() { baseControls->undoSeq(); }
		virtual void erase() { baseControls->erase(); }
		virtual void setSlider(int i) { baseControls->setSlider(i); }

		virtual bool isTypable() { return baseControls->isTypable(); }

		int getSoundIncrement(int notch) { return baseControls->getSoundIncrement(notch); }
		void setSliderNoteVar(mpc::sequencer::NoteEvent* n, std::weak_ptr<mpc::sampler::Program> program) { baseControls->setSliderNoteVar(n, program); }

		virtual void pad(int i, int velo, bool repeat, int tick) { baseControls->pad(i, velo, repeat, tick); }

	};
}

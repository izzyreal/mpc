#pragma once
#include "Component.hpp"
#include <Observer.hpp>

#include <controls/BaseControls.hpp>

#include "Screens.hpp"
#include "Field.hpp"
#include "Label.hpp"
#include "FunctionKeys.hpp"
#include "Background.hpp"
#include "Wave.hpp"
#include "EnvGraph.hpp"

#include <StrUtil.hpp>

#include <memory>
#include <string>
#include <map>

#include "engine/Drum.hpp"
#include "sequencer/Track.hpp"

namespace mpc::lcdgui
{
	class ScreenComponent
		: public Component, public Observer
	{

	private:
		const int layer;
		std::map<std::string, std::vector<std::string>> transferMap;
		std::string firstField;

	public:
		const int& getLayerIndex();
		
	protected:
		mpc::Mpc& mpc;
		std::shared_ptr<mpc::sampler::Sampler> sampler;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::shared_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::string param;
		std::shared_ptr<mpc::sampler::Program> program;
		std::shared_ptr<mpc::sequencer::Track> track;
		mpc::engine::Drum& activeDrum();

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
		virtual void init()
        {
			param = controls::BaseControls::getFocusedFieldName(mpc);
            program = sampler->getProgram(activeDrum().getProgram());
			track = mpc.getSequencer()->getActiveTrack();
		}

	public:
		virtual void left() { mpc::controls::BaseControls::left(mpc); }
		virtual void right() { mpc::controls::BaseControls::right(mpc); }
		virtual void up() { mpc::controls::BaseControls::up(mpc); }
		virtual void down() { mpc::controls::BaseControls::down(mpc); }
		virtual void function(int i) { init(); mpc::controls::BaseControls::function(mpc, i); }
		virtual void openWindow();
		virtual void turnWheel(int i) {}
		virtual void numpad(int i) { mpc::controls::BaseControls::numpad(mpc, i); }
		virtual void pressEnter() { mpc::controls::BaseControls::pressEnter(mpc); }
		virtual void rec() { mpc::controls::BaseControls::rec(mpc); }
		virtual void overDub() { mpc::controls::BaseControls::overDub(mpc); }
		virtual void stop() { mpc::controls::BaseControls::stop(mpc); }
		virtual void play() { mpc::controls::BaseControls::play(mpc); }
		virtual void playStart() { mpc::controls::BaseControls::playStart(mpc); }
		virtual void mainScreen() { mpc::controls::BaseControls::mainScreen(mpc); }
		virtual void tap() { mpc::controls::BaseControls::tap(mpc); }
		virtual void prevStepEvent() { }
		virtual void nextStepEvent() { }
		virtual void goTo() { mpc::controls::BaseControls::goTo(mpc); }
		virtual void prevBarStart() { }
		virtual void nextBarEnd() { }
		virtual void nextSeq() { mpc::controls::BaseControls::nextSeq(mpc); }
		virtual void trackMute() { mpc::controls::BaseControls::trackMute(mpc); }
		virtual void bank(int i) { mpc::controls::BaseControls::bank(mpc, i); }
		virtual void fullLevel() { mpc::controls::BaseControls::fullLevel(mpc); }
		virtual void sixteenLevels() { mpc::controls::BaseControls::sixteenLevels(mpc); }
		virtual void after() { mpc::controls::BaseControls::after(mpc); }
		virtual void shift() { mpc::controls::BaseControls::shift(mpc); }
		virtual void undoSeq() { mpc::controls::BaseControls::undoSeq(mpc); }
		virtual void erase() { mpc::controls::BaseControls::erase(mpc); }
		virtual void setSlider(int i) { }

		int getSoundIncrement(int notch) { return mpc::controls::BaseControls::getSoundIncrement(mpc.getSampler()->getSound()->getFrameCount(), notch); }

		virtual void pad(int padIndexWithBank, int velo);

	};
}

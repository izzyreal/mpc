#pragma once
#include "Component.hpp"
#include <Observer.hpp>

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
#include "sampler/Sampler.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "command/AllCommands.h"

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
			param = mpc.getLayeredScreen()->getFocus();
            program = sampler->getProgram(activeDrum().getProgram());
			track = mpc.getSequencer()->getActiveTrack();
		}

	public:
        virtual void left() { command::LeftCommand(mpc).execute(); }
        virtual void right() { command::RightCommand(mpc).execute(); }
        virtual void up() { command::UpCommand(mpc).execute(); }
        virtual void down() { command::DownCommand(mpc).execute(); }
        virtual void function(int i) { init(); command::FunctionCommand(mpc, i).execute(); }
        virtual void openWindow();
        virtual void turnWheel(int i) {}
        virtual void numpad(int i) { command::NumPadCommand(mpc, i).execute(); }
        virtual void pressEnter() { command::PressEnterCommand(mpc).execute(); }
        virtual void rec() { command::RecCommand(mpc).execute(); }
        virtual void overDub() { command::OverdubCommand(mpc).execute(); }
        virtual void stop() { command::StopCommand(mpc).execute(); }
        virtual void play() { command::PlayStartCommand(mpc).execute(); }
        virtual void playStart() { command::PlayStartCommand(mpc).execute(); }
        virtual void mainScreen() { command::MainScreenCommand(mpc).execute(); }
        virtual void tap() { command::TapCommand(mpc).execute(); }
        virtual void prevStepEvent() {}
        virtual void nextStepEvent() {}
        virtual void goTo() { command::GoToCommand(mpc).execute(); }
        virtual void prevBarStart() {}
        virtual void nextBarEnd() {}
        virtual void nextSeq() { command::NextSeqCommand(mpc).execute(); }
        virtual void trackMute() { command::TrackMuteCommand(mpc).execute(); }
        virtual void bank(int i) { command::BankCommand(mpc, i).execute(); }
        virtual void fullLevel() { command::FullLevelCommand(mpc).execute(); }
        virtual void sixteenLevels() { command::SixteenLevelsCommand(mpc).execute(); }
        virtual void after() { command::AfterCommand(mpc).execute(); }
        virtual void shift() { command::ShiftCommand(mpc).execute(); }
        virtual void undoSeq() { command::UndoSeqCommand(mpc).execute(); }
        virtual void erase() { command::EraseCommand(mpc).execute(); }
        virtual void setSlider(int i) {}

		int getSoundIncrement(const int dataWheelSteps)
        {
            auto result = dataWheelSteps;
            
            if (std::abs(result) != 1)
            {
                result *= (int)(ceil(mpc.getSampler()->getSound()->getFrameCount() / 15000.f));
            }
            
            return result;
        }

		virtual void pad(int padIndexWithBank, int velo);

	};
}


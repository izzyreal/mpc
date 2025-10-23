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

#include "command/AllCommands.hpp"

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

        std::optional<int> getDrumIndex();
		
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
		void setLastFocus(const std::string& screenName, const std::string& newLastFocus);
		const std::string getLastFocus(const std::string& screenName);
		std::shared_ptr<Wave> findWave();
		std::shared_ptr<EnvGraph> findEnvGraph();

	public:
		virtual void open() {}
		virtual void close() {}

	public:
		ScreenComponent(mpc::Mpc& mpc, const std::string& name, const int layer);
        std::shared_ptr<Field> findFocus();
		void setTransferMap(const std::map<std::string, std::vector<std::string>>& newTransferMap);
		void setFirstField(const std::string& newFirstField);
		std::string getFirstField();
		std::map<std::string, std::vector<std::string>>& getTransferMap();

	protected:
		virtual void init()
        {
			param = mpc.getLayeredScreen()->getFocusedFieldName();
            program = sampler->getProgram(activeDrum().getProgram());
			track = mpc.getSequencer()->getActiveTrack();
		}

	public:
        virtual void left() { command::PushLeftCommand(mpc).execute(); }
        virtual void right() { command::PushRightCommand(mpc).execute(); }
        virtual void up() { command::PushUpCommand(mpc).execute(); }
        virtual void down() { command::PushDownCommand(mpc).execute(); }
        virtual void function(int i) { if (i == 3) mpc.getLayeredScreen()->closeCurrentScreen(); }
        virtual void openWindow();
        virtual void turnWheel(int) {}
        virtual void numpad(int i) { command::PushNumPadCommand(mpc, i).execute(); }
        virtual void pressEnter() { command::PushEnterCommand(mpc).execute(); }
        virtual void rec() { command::PushRecCommand(mpc).execute(); }
        virtual void overDub() { command::PushOverdubCommand(mpc).execute(); }
        virtual void stop() { command::PushStopCommand(mpc).execute(); }
        virtual void play() { command::PushPlayCommand(mpc).execute(); }
        virtual void playStart() { command::PushPlayStartCommand(mpc).execute(); }
        virtual void setSlider(int) {}

		int getSoundIncrement(const int dataWheelSteps)
        {
            auto result = dataWheelSteps;
            
            if (std::abs(result) != 1)
            {
                result *= (int)(ceil(mpc.getSampler()->getSound()->getFrameCount() / 15000.f));
            }
            
            return result;
        }
	};
}


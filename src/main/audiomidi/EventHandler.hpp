#pragma once
#include <observer/Observable.hpp>
#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MidiAdapter.hpp>

#include <memory>

namespace mpc::audiomidi {

	class EventHandler final
		: public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		
	public:
		void handle(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track);
		void handleNoThru(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track, int timeStamp);

	private:
		void midiOut(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track);

	public:
		EventHandler(mpc::Mpc& mpc);

	};
}

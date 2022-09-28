#pragma once
#include <observer/Observable.hpp>
#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MidiAdapter.hpp>

#include <memory>
#include <map>

namespace mpc::audiomidi {

	class EventHandler final
		: public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		std::map<std::pair<int, int>, int> transposeCache;

	public:
		void handle(const std::weak_ptr<mpc::sequencer::Event>& event, mpc::sequencer::Track* track);
		void handleNoThru(const std::weak_ptr<mpc::sequencer::Event>& event, mpc::sequencer::Track* track, int timeStamp);

	private:
		void midiOut(const std::weak_ptr<mpc::sequencer::Event> &event, mpc::sequencer::Track* track);

	public:
		EventHandler(mpc::Mpc& mpc);

	};
}

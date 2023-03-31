#pragma once
#include <observer/Observable.hpp>
#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>

#include <memory>
#include <map>

namespace mpc::audiomidi {

	class EventHandler final
		: public moduru::observer::Observable
	{

	private:
		mpc::Mpc& mpc;
		std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
		std::shared_ptr<mpc::sampler::Sampler> sampler;
		std::map<std::shared_ptr<mpc::sequencer::NoteOffEvent>, int> transposeCache;

	public:
		void handle(
                const std::shared_ptr<mpc::sequencer::Event>& event,
                mpc::sequencer::Track* track,
                char drum = -1);
		void handleNoThru(const std::shared_ptr<mpc::sequencer::Event>& event,
                          mpc::sequencer::Track* track,
                          int timeStamp,
                          char drum = -1);

	private:
		void midiOut(const std::shared_ptr<mpc::sequencer::Event> &event, mpc::sequencer::Track* track);

	public:
		EventHandler(mpc::Mpc& mpc);

	};
}

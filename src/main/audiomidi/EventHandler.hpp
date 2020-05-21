#pragma once
#include <observer/Observable.hpp>
#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MidiAdapter.hpp>

#include <memory>

namespace mpc::ui::midisync {
	class MidiSyncGui;
}

namespace mpc::ui::vmpc {
	class MidiGui;
}


namespace mpc::audiomidi {

	class EventHandler final
		: public moduru::observer::Observable
	{

	private:
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		mpc::ui::midisync::MidiSyncGui* msGui = nullptr;
		ui::vmpc::MidiGui* midiGui = nullptr;
		
	public:
		void handle(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track);
		void handleNoThru(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track, int timeStamp);

	private:
		void midiOut(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track);

	public:
		EventHandler();

	};
}

#pragma once
#include <Observer.hpp>
#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>

#include <memory>
#include <optional>
#include <map>

namespace mpc::audiomidi {

	class EventHandler final
		: public Observable
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
                std::optional<uint8_t> drum = std::optional<uint8_t>());
		void handleNoThru(const std::shared_ptr<mpc::sequencer::Event>& event,
                          mpc::sequencer::Track* track,
                          int timeStamp,
                          std::optional<uint8_t> clientDrumIndex = std::optional<uint8_t>());

	private:
		void midiOut(const std::shared_ptr<mpc::sequencer::Event> &event, mpc::sequencer::Track* track);

        void handleDrumEvent(int timeStamp,
                             const std::shared_ptr<mpc::sequencer::NoteOnEvent>& noteOnEvent,
                             const std::shared_ptr<mpc::sequencer::NoteOffEvent>& noteOffEvent,
                             uint8_t drumIndex,
                             mpc::sequencer::Track* track);

	public:
		EventHandler(mpc::Mpc& mpc);

	};
}

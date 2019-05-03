#pragma once
#include <observer/Observable.hpp>
#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MidiAdapter.hpp>

#include <memory>

namespace mpc {

	namespace ui {
		namespace sequencer {
			namespace window {
				class SequencerWindowGui;
			}
		}
		namespace midisync {
			class MidiSyncGui;
		}
		namespace vmpc {
			class MidiGui;
		}
	}


	namespace audiomidi {

		class EventHandler final
			: public moduru::observer::Observable
		{

		private:
			std::weak_ptr<mpc::sequencer::Sequencer> sequencer{ };
			std::weak_ptr<mpc::sampler::Sampler> sampler{};
			ui::sequencer::window::SequencerWindowGui* swGui{ nullptr };
			mpc::ui::midisync::MidiSyncGui* msGui{ nullptr };
			ui::vmpc::MidiGui* midiGui{ nullptr };
			Mpc* mpc{ nullptr };
			//mpc::sequencer::MidiAdapter midiAdapter;

		public:
			void handle(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track);
			void handleNoThru(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track, int timeStamp);

		private:
			void midiOut(std::weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track);

		public:
			EventHandler(Mpc* mpc);

		};

	}
}

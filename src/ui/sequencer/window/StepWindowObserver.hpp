#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>

namespace ctoot {
	namespace mpc {
		class MpcSoundPlayerChannel;
	}
}

namespace mpc {

	class Mpc;

	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace sequencer {
		class Sequencer;
		class Sequence;
		class Track;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}
	namespace ui {
		namespace sampler {
			class SamplerGui;
		}
		namespace sequencer {

			class StepEditorGui;

			namespace window {

				class SequencerWindowGui;

				class StepWindowObserver
					: public moduru::observer::Observer
				{

				private:
					std::vector<std::string> timingCorrectNames{};
					std::vector<std::string> eventTypeNames{};
					std::vector<std::string> noteVariationParameterNames{};
					std::vector<std::string> editTypeNames{};
					std::vector<std::string> singleLabels{};
					std::vector<int> xPosDouble{};
					std::vector<int> yPosDouble{};
					std::vector<std::string> doubleLabels{};

					mpc::Mpc* mpc;
					std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
					std::weak_ptr<mpc::sequencer::Track> track{};
					std::weak_ptr<mpc::lcdgui::Field> tcValueField{};
					std::weak_ptr<mpc::lcdgui::Field> eventtypeField{};
					std::weak_ptr<mpc::lcdgui::Field> editMultiValue0Field{};
					std::weak_ptr<mpc::lcdgui::Field> editMultiValue1Field{};
					std::weak_ptr<mpc::lcdgui::Label> editMultiParam0Label{};
					std::weak_ptr<mpc::lcdgui::Label> editMultiParam1Label{};
					mpc::ui::sequencer::StepEditorGui* seqGui{};
					mpc::ui::sequencer::window::SequencerWindowGui* swGui;
					int xPosSingle{};
					int yPosSingle{};
					std::weak_ptr<mpc::sampler::Sampler> sampler{};
					std::weak_ptr<mpc::sampler::Program> program{};
					ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{};
					mpc::ui::sampler::SamplerGui* samplerGui{};

				public:
					void update(moduru::observer::Observable* o, boost::any arg) override;

				private:
					void updateEditMultiple();
					void updateDouble();

				public:
					StepWindowObserver(mpc::Mpc* mpc);
					~StepWindowObserver();

				};

			}
		}
	}
}

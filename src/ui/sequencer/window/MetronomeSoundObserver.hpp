#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>

namespace mpc {
	class Mpc;
	namespace sampler {
		class Sampler;
	}

	namespace lcdgui {
		class Field;
		class Label;
		class Background;
	}

	namespace ui {

		namespace sequencer {
			namespace window {

				class SequencerWindowGui;

				class MetronomeSoundObserver
					: public moduru::observer::Observer
				{

				private:
					std::weak_ptr<mpc::sampler::Sampler> sampler{};
					std::vector<std::string> soundNames{};
					std::weak_ptr<mpc::lcdgui::Field> soundField{};
					std::weak_ptr<mpc::lcdgui::Field> volumeField{};
					std::weak_ptr<mpc::lcdgui::Label> volumeLabel{};
					std::weak_ptr<mpc::lcdgui::Field> outputField{};
					std::weak_ptr<mpc::lcdgui::Label> outputLabel{};
					std::weak_ptr<mpc::lcdgui::Field> accentField{};
					std::weak_ptr<mpc::lcdgui::Label> accentLabel{};
					std::weak_ptr<mpc::lcdgui::Field> normalField{};
					std::weak_ptr<mpc::lcdgui::Label> normalLabel{};
					std::weak_ptr<mpc::lcdgui::Field> veloAccentField{};
					std::weak_ptr<mpc::lcdgui::Label> veloAccentLabel{};
					std::weak_ptr<mpc::lcdgui::Field> veloNormalField{};
					std::weak_ptr<mpc::lcdgui::Label> veloNormalLabel{};
					SequencerWindowGui* swGui{ nullptr };
					mpc::lcdgui::Background* bg{ nullptr };

				private:
					void displaySound();
					void displayVolume();
					void displayOutput();
					void displayAccent();
					void displayNormal();
					void displayVelocityAccent();
					void displayVelocityNormal();

				public:
					void update(moduru::observer::Observable* o, boost::any arg) override;

					MetronomeSoundObserver(mpc::Mpc* mpc);
					~MetronomeSoundObserver();

				};

			}
		}
	}
}

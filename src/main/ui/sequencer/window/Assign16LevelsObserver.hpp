#pragma once
#include <observer/Observer.hpp>
#include <lcdgui/Field.hpp>

#include <vector>
#include <string>

namespace mpc {

	

	namespace ui {
		namespace sequencer {

			class SequencerGui;

			namespace window {

				class Assign16LevelsObserver
					: public moduru::observer::Observer
				{

				public:
					static std::vector<std::string> TYPE_NAMES;

				private:
					static std::vector<std::string> PARAM_NAMES;

					
					std::weak_ptr<mpc::lcdgui::Field> noteField{};
					std::weak_ptr<mpc::lcdgui::Field> paramField{};
					std::weak_ptr<mpc::lcdgui::Field> typeField{};
					std::weak_ptr<mpc::lcdgui::Field> originalKeyPadField{};
					mpc::ui::sequencer::SequencerGui* sGui{ nullptr };
					std::weak_ptr<mpc::lcdgui::Label> typeLabel{};
					std::weak_ptr<mpc::lcdgui::Label> originalKeyPadLabel{};

				private:
					void displayNote();
					void displayParameter();
					void displayType();
					void displayOriginalKeyPad();

				public:
					void update(moduru::observer::Observable* o, nonstd::any arg) override;

					Assign16LevelsObserver();
					~Assign16LevelsObserver();

				};

			}
		}
	}
}

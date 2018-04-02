#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	class Mpc;
	namespace sampler {
		class Sampler;
		class Sound;
	}

	namespace lcdgui {
		class Wave;
		class Field;
		class Label;
		class TwoDots;
	}

	namespace ui {
	
		namespace sampler {

			class SoundGui;

			namespace window {

				class ZoomGui;

				class ZoomObserver
					: public moduru::observer::Observer
				{


				private:
					//std::vector<float> trimmedData;
					ZoomGui* zoomGui{ nullptr };
					mpc::ui::sampler::SoundGui* soundGui{ nullptr };
					std::weak_ptr<mpc::lcdgui::TwoDots> twoDots;
					std::weak_ptr<mpc::lcdgui::Field> startField{};
					std::weak_ptr<mpc::lcdgui::Field> endField{};
					std::weak_ptr<mpc::lcdgui::Field> toField{};
					std::weak_ptr<mpc::lcdgui::Field> smplLngthField{};
					std::weak_ptr<mpc::lcdgui::Field> loopLngthField{};
					std::weak_ptr<mpc::lcdgui::Field> playXField{};
					std::weak_ptr<mpc::lcdgui::Label> lngthLabel{};
					std::weak_ptr<mpc::lcdgui::Field> lngthField{};
					std::weak_ptr<mpc::sampler::Sound> sound{};
					std::vector<std::string> playXNames{};
					std::weak_ptr<mpc::lcdgui::Wave> wave;
					std::string csn{ "" };

				private:
					void displayLoopLngth();
					void displayLngthField();
					void displayFineWaveform();
					void displayStart();
					void displayEnd();
					void displayLngthLabel();
					void displaySmplLngth();
					void displayPlayX();

				public:
					void update(moduru::observer::Observable* o, boost::any arg) override;

				private:
					void displayTo();

				public:
					ZoomObserver(mpc::Mpc* mpc);
					~ZoomObserver();
				};
	
			}
		}
	}
}

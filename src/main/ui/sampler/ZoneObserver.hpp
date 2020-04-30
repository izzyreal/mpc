#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	
	namespace sampler {
		class Sampler;
		class Sound;
	}

	namespace lcdgui {
		class Field;
		class Wave;
		class TwoDots;
	}

	namespace ui {

		namespace sampler {

			class SoundGui;

			class ZoneObserver
				: public moduru::observer::Observer
			{

			private:
				
				SoundGui* soundGui{ nullptr };
				std::vector<std::string> playXNames{};
				std::weak_ptr<mpc::sampler::Sampler> sampler{ };
				std::weak_ptr<mpc::sampler::Sound> sound{};
				std::weak_ptr<mpc::lcdgui::Field> sndField{};
				std::weak_ptr<mpc::lcdgui::Field> playXField{};
				std::weak_ptr<mpc::lcdgui::Field> stField{};
				std::weak_ptr<mpc::lcdgui::Field> endField{};
				std::weak_ptr<mpc::lcdgui::Field> zoneField{};
				std::weak_ptr<mpc::lcdgui::Field> dummyField{};
				std::weak_ptr<mpc::lcdgui::TwoDots> twoDots;
				std::weak_ptr<mpc::lcdgui::Wave> wave;
				std::weak_ptr<mpc::lcdgui::Field> numberOfZonesField{};
				std::string csn{ "" };

			private:
				void displaySnd();
				void displayPlayX();
				void displaySt();
				void displayEnd();
				void displayZone();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			private:
				void displayNumberOfZones();
				void waveformLoadData();

			public:
				ZoneObserver();
				~ZoneObserver();

			};

		}
	}
}

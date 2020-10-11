#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>

namespace mpc::sampler { class Sampler; }

namespace mpc::lcdgui
{
	class Field;
	class Label;
	class Background;
}

namespace mpc::lcdgui::screens::dialog
{
	class MetronomeSoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		MetronomeSoundScreen(mpc::Mpc& mpc, const int layerIndex);

		void turnWheel(int notch) override;
		void function(int i) override;
		void open() override;
		void close() override;

		void update(moduru::observer::Observable* o, nonstd::any msg) override;

	private:
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		std::vector<std::string> soundNames = { "CLICK", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };

	private:
		void displaySound();
		void displayVolume();
		void displayOutput();
		void displayAccent();
		void displayNormal();
		void displayAccentVelo();
		void displayNormalVelo();

	private:
		int volume = 100;
		int output = 0;
		int sound = 0;
		int accentVelo = 127;
		int accentPad = 35;
		int normalVelo = 64;
		int normalPad = 35;

	public:
		int getVolume();
		void setVolume(int i);
		int getOutput();
		void setOutput(int i);
		int getSound();
		void setSound(int i);
		int getAccentPad();
		void setAccentPad(int i);
		int getAccentVelo();
		void setAccentVelo(int i);
		int getNormalPad();
		void setNormalPad(int i);
		int getNormalVelo();
		void setNormalVelo(int i);

	};
}

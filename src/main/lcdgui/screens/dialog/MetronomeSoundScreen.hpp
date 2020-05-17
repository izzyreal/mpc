#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>

namespace mpc::sampler
{
	class Sampler;
}

namespace mpc::lcdgui
{
	class Field;
	class Label;
	class Background;
}

namespace mpc::lcdgui::screens::dialog {

	class MetronomeSoundScreen
		: public mpc::lcdgui::ScreenComponent
	{

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
		int clickVolume = 100;
		int clickOutput = 0;
		int metronomeSound = 0;
		int accentVelo = 127;
		int accentNote = 35;
		int normalVelo = 64;
		int normalNote = 35;

	public:
		int getClickVolume();
		void setClickVolume(int i);
		int getClickOutput();
		void setClickOutput(int i);
		int getMetronomeSound();
		void setMetronomeSound(int i);
		int getAccentNote();
		void setAccentNote(int i);
		int getAccentVelo();
		void setAccentVelo(int i);
		int getNormalNote();
		void setNormalNote(int i);
		int getNormalVelo();
		void setNormalVelo(int i);

	public:
		MetronomeSoundScreen(const int layer);

	public:
		void turnWheel(int notch) override;
		void function(int i) override;

	public:
		void open() override;

	};
}

#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class MidiInputScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		MidiInputScreen(mpc::Mpc& mpc, const int layerIndex);

	public:
		void open() override;

	private:
		void displayPass();
		void displayType();
		void displayMidiFilter();
		void displaySustainPedalToDuration();
		void displayProgChangeSeq();
		void displayReceiveCh();

	public:
		int getReceiveCh();
		bool getProgChangeSeq();
		bool isSustainPedalToDurationEnabled();
		bool isMidiFilterEnabled();
		int getType();
		bool getPass();
		void setReceiveCh(int i);
		void setProgChangeSeq(bool b);
		void setSustainPedalToDuration(bool b);
		void setMidiFilterEnabled(bool b);
		void setType(int i);
		void setPass(bool b);
		bool isNotePassEnabled();
		void setNotePassEnabled(bool b);
		bool isPitchBendPassEnabled();
		void setPitchBendPassEnabled(bool b);
		bool isPgmChangePassEnabled();
		void setPgmChangePassEnabled(bool b);
		bool isChPressurePassEnabled();
		void setChPressurePassEnabled(bool b);
		bool isPolyPressurePassEnabled();
		void setPolyPressurePassEnabled(bool b);
		bool isExclusivePassEnabled();
		void setExclusivePassEnabled(bool b);

	private:
		const std::vector<std::string> typeNames = { "NOTES", "PITCH BEND", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE", "BANK SEL MSB", "MOD WHEEL", "BREATH CONT", "03", "FOOT CONTROL", "PORTA TIME", "DATA ENTRY", "MAIN VOLUME", "BALANCE", "09", "PAN", "EXPRESSION", "EFFECT 1"	, "EFFECT 2", "14", "15", "GEN.PUR. 1", "GEN.PUR. 2", "GEN.PUR. 3", "GEN.PUR. 4", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "BANK SEL LSB", "MOD WHEL LSB", "BREATH LSB", "35", "FOOT CNT LSB", "PORT TIME LS", "DATA ENT LSB", "MAIN VOL LSB", "BALANCE LSB", "41", "PAN LSB", "EXPRESS LSB", "EFFECT 1 LSB", "EFFECT 2 MSB", "46", "47", "GEN.PUR.1 LS", "GEN.PUR.2 LS", "GEN.PUR.3 LS", "GEN.PUR.4 LS", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "SUSTAIN PDL", "PORTA PEDAL", "SOSTENUTO", "SOFT PEDAL", "LEGATO FT SW", "HOLD 2", "SOUND VARI", "TIMBER/HARMO", "RELEASE TIME", "ATTACK TIME", "BRIGHTNESS", "SOUND CONT 6", "SOUND CONT 7", "SOUND CONT 8", "SOUND CONT 9", "SOUND CONT10", "GEN.PUR. 5", "GEN.PUR. 6", "GEN.PUR. 7", "GEN.PUR. 8", "PORTA CNTRL", "85", "86", "87", "88", "89", "90", "EXT EFF DPTH", "TREMOLO DPTH", "CHORUS DEPTH", " DETUNE DEPTH", "PHASER DEPTH", "DATA INCRE", "DATA DECRE", "NRPN LSB", "NRPN MSB", "RPN LSB", "RPN MSB", "102", "103", "104", "105", "106", "107" "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "ALL SND OFF", "RESET CONTRL", "LOCAL ON/OFF", "ALL NOTE OFF", "OMNI OFF", "OMNI ON", "MONO MODE ON", "POLY MODE ON" };
		bool pass = true;
		int type = 0;
		bool midiFilter = false;
		int receiveCh = -1; // Default receive channel is -1 or "ALL"
		bool sustainPedalToDuration = true;
		bool progChangeSeq = false;

		// These should be in a map<string, boolean> with typeNames as keys
		bool notePassEnabled = false;
		bool pitchBendPassEnabled = false;
		bool pgmChangePassEnabled = false;
		bool chPressurePassEnabled = false;
		bool polyPressurePassEnabled = false;
		bool exclusivePassEnabled = false;

	};
}

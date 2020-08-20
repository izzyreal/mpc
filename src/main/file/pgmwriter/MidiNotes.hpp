#pragma once

#include <vector>

#ifdef __linux__
#include <cstdint>
#endif // __linux__

namespace mpc::sampler {
		class Program;
}

namespace mpc::file::pgmwriter
{

	class MidiNotes
	{

	public:
		std::vector<char> midiNotesArray;

	private:
		void setSampleSelect(int midiNote, int sampleNumber);
		void setSoundGenerationMode(int midiNote, int soundGenerationMode);
		void setVelocityRangeLower(int midiNote, int velocityRangeLower);
		void setAlsoPlayUse1(int midiNote, int alsoPlayUse1);
		void setVelocityRangeUpper(int midiNote, int velocityRangeUpper);
		void setAlsoPlayUse2(int midiNote, int alsoPlayUse2);
		void setVoiceOverlap(int midiNote, int voiceOverlap);
		void setMuteAssign1(int midiNote, int muteAssign1);
		void setMuteAssign2(int midiNote, int muteAssign2);
		void setTune(int midiNote, int16_t tune);
		void setAttack(int midiNote, int attack);
		void setDecay(int midiNote, int decay);
		void setDecayMode(int midiNote, int decayMode);
		void setCutoff(int midiNote, int cutoff);
		void setResonance(int midiNote, int resonance);
		void setVelEnvToFiltAtt(int midiNote, int velEnvToFiltAtt);
		void setVelEnvToFiltDec(int midiNote, int velEnvToFiltDec);
		void setVelEnvToFiltAmt(int midiNote, int velEnvToFiltAmt);
		void setVelocityToLevel(int midiNote, int velocityToLevel);
		void setVelocityToAttack(int midiNote, int velocityToAttack);
		void setVelocityToStart(int midiNote, int velocityToStart);
		void setVelocityToCutoff(int midiNote, int velocityToCutoff);
		void setSliderParameter(int midiNote, int sliderParameter);
		void setVelocityToPitch(int midiNote, int velocityToPitch);
		void setShort(std::vector<char>& ca, int offset, int16_t s);

	public:
		MidiNotes(mpc::sampler::Program* program, std::vector<int> snConvTable);
	};
}

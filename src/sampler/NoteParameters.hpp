#pragma once

#include <observer/Observable.hpp>

namespace mpc {

	namespace sampler {

		class Sampler;

		class NoteParameters
			: public moduru::observer::Observable
		{

		private:
			int soundNumber{ -1 };
			int soundGenerationMode{ 0 };
			int velocityRangeLower{ 0 };
			int optionalNoteA{ 0 };
			int velocityRangeUpper{ 0 };
			int optionalNoteB{ 0 };
			int voiceOverlap{ 0 };
			int muteAssignA{ 0 };
			int muteAssignB{ 0 };
			int tune{ 0 };
			int attack{ 0 };
			int decay{ 0 };
			int decayMode{ 0 };
			int filterFrequency{ 0 };
			int filterResonance{ 0 };
			int filterAttack{ 0 };
			int filterDecay{ 0 };
			int filterEnvelopeAmount{ 0 };
			int velocityToLevel{ 0 };
			int velocityToAttack{ 0 };
			int velocityToStart{ 0 };
			int velocityToFilterFrequency{ 0 };
			int sliderParameterNumber{ 0 };
			int velocityToPitch{ 0 };
			int number{ 0 };

		public:
			int getSndNumber();
			void setSoundNumberNoLimit(int i);
			void setSoundNumber(int i);
			void setSoundGenMode(int i);
			int getSoundGenerationMode();
			void setVeloRangeLower(int i);
			int getVelocityRangeLower();
			void setOptNoteA(int i);
			int getOptionalNoteA();
			void setVeloRangeUpper(int i);
			int getVelocityRangeUpper();
			void setOptionalNoteB(int i);
			int getOptionalNoteB();
			int getVoiceOverlap();
			void setVoiceOverlap(int i);
			void setMuteAssignA(int i);
			int getMuteAssignA();
			void setMuteAssignB(int i);
			int getMuteAssignB();
			void setTune(int i);
			int getTune();
			void setAttack(int i);
			int getAttack();
			void setDecay(int i);
			int getDecay();
			void setDecayMode(int i);
			int getDecayMode();
			void setFilterFrequency(int i);
			int getFilterFrequency();
			void setFilterResonance(int i);
			int getFilterResonance();
			void setFilterAttack(int i);
			int getFilterAttack();
			void setFilterDecay(int i);
			int getFilterDecay();
			void setFilterEnvelopeAmount(int i);
			int getFilterEnvelopeAmount();
			void setVeloToLevel(int i);
			int getVeloToLevel();
			void setVelocityToAttack(int i);
			int getVelocityToAttack();
			void setVelocityToStart(int i);
			int getVelocityToStart();
			void setVelocityToFilterFrequency(int i);
			int getVelocityToFilterFrequency();
			void setSliderParameterNumber(int i);
			int getSliderParameterNumber();
			void setVelocityToPitch(int i);
			int getVelocityToPitch();
			NoteParameters* clone();
			int getNumber();

			NoteParameters(int number);
			~NoteParameters();
		};
	}
}

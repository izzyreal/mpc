#pragma once
#include "engine/MpcProgram.hpp"

#include <sampler/NoteParameters.hpp>
#include <sampler/PgmSlider.hpp>

#include <memory>

namespace mpc::engine {
	class MpcSampler;
	class StereoMixer;
	class IndivFxMixer;
	class MpcNoteParameters;
}

namespace mpc { class Mpc; }

namespace mpc::sampler {

	class Pad;

	class Program
		: public virtual mpc::engine::MpcProgram
	{

	public:
		std::shared_ptr<mpc::engine::StereoMixer> getStereoMixerChannel(int noteIndex) override;
		std::shared_ptr<mpc::engine::IndivFxMixer> getIndivFxMixerChannel(int noteIndex) override;
        
		int getPadIndexFromNote(int note);
		
        mpc::engine::MpcNoteParameters* getNoteParameters(int i) override;

	private:
		Sampler* sampler = nullptr;
		std::string name = "";
		std::vector<NoteParameters*> noteParameters;
		std::vector<Pad*> pads;
		PgmSlider* slider = nullptr;
		int midiProgramChange = 0;

		void init();

	public:
		int getNumberOfSamples();
		void setName(std::string s);
		std::string getName();
		Pad* getPad(int i);
		std::vector<NoteParameters*> getNotesParameters();
		mpc::sampler::PgmSlider* getSlider();
		void setNoteParameters(int i, NoteParameters* noteParameters);
		int getMidiProgramChange();
		void setMidiProgramChange(int i);
		void initPadAssign();
		int getNoteFromPad(int i);
		std::vector<int> getPadIndicesFromNote(const int note);

	public:
		Program(mpc::Mpc& mpc, mpc::sampler::Sampler* samplerToUse);
		~Program();

	};
}

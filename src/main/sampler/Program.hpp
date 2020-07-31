#pragma once
#include <mpc/MpcProgram.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/PgmSlider.hpp>

#include <observer/Observable.hpp>

#include <memory>

namespace ctoot::mpc {
	class MpcSampler;
	class MpcStereoMixerChannel;
	class MpcIndivFxMixerChannel;
	class MpcNoteParameters;
}

namespace mpc { class Mpc; }

namespace mpc::sampler {

	class Pad;

	class Program
		: public virtual ctoot::mpc::MpcProgram
		, public moduru::observer::Observable
	{

	public:
		std::weak_ptr<ctoot::mpc::MpcStereoMixerChannel> getStereoMixerChannel(int pad) override;
		std::weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel> getIndivFxMixerChannel(int pad) override;
		int getPadIndexFromNote(int note) override;
		ctoot::mpc::MpcNoteParameters* getNoteParameters(int i) override;

	private:
		Sampler* sampler{ nullptr };
		std::string name = "";
		std::vector<NoteParameters*> noteParameters{ };
		std::vector<Pad*> pads{ };
		PgmSlider* slider{ nullptr };
		int midiProgramChange{ 0 };

		void init();

	public:
		int getNumberOfSamples();
		void setName(std::string s);
		std::string getName();
		Pad* getPad(int i);
		std::vector<NoteParameters*> getNotesParameters();
		mpc::sampler::PgmSlider* getSlider();
		void setNoteParameters(int i, NoteParameters* nn);
		int getMidiProgramChange();
		void setMidiProgramChange(int i);
		void initPadAssign();
		int getNoteFromPad(int i);

	public:
		Program(mpc::Mpc& mpc, mpc::sampler::Sampler* sampler);
		~Program();

	};
}

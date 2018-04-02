#pragma once
#include <sampler/NoteParameters.hpp>
#include <sampler/PgmSlider.hpp>

#include <observer/Observable.hpp>

#include <memory>

namespace mpc {

	class Mpc;

	namespace sampler {

		class StereoMixerChannel;
		class IndivFxMixerChannel;
		class Pad;
		class Sampler;

		class Program
			: public moduru::observer::Observable
		{

		private:
			Sampler* sampler{ nullptr };
			std::string name{ "" };
			std::vector<NoteParameters*> noteParameters{ };
			std::vector<Pad*> pads{ };
			PgmSlider* slider{ nullptr };
			int midiProgramChange{ 0 };

			void init();

		public:
			int getNumberOfSamples();
			void setName(std::string s);
			std::string getName();
			NoteParameters* getNoteParameters(int i);
			Pad* getPad(int i);
			std::weak_ptr<StereoMixerChannel> getStereoMixerChannel(int pad);
			std::weak_ptr<IndivFxMixerChannel> getIndivFxMixerChannel(int pad);
			int getPadNumberFromNote(int note);
			std::vector<NoteParameters*> getNotesParameters();
			mpc::sampler::PgmSlider* getSlider();
			void setNoteParameters(int i, NoteParameters* nn);
			int getMidiProgramChange();
			void setMidiProgramChange(int i);
			void initPadAssign();
			int getNoteFromPad(int i);

		public:
			Program(mpc::Mpc* mpc, mpc::sampler::Sampler* sampler);
			~Program();

		};

	}
}

#pragma once

#include <sampler/NoteParameters.hpp>
#include <sampler/PgmSlider.hpp>

#include <memory>

namespace mpc::engine {
	class StereoMixer;
	class IndivFxMixer;
}

namespace mpc { class Mpc; }

namespace mpc::sampler {

	class Pad;

	class Program
	{

	public:
		std::shared_ptr<mpc::engine::StereoMixer> getStereoMixerChannel(int noteIndex);
		std::shared_ptr<mpc::engine::IndivFxMixer> getIndivFxMixerChannel(int noteIndex);
        
		int getPadIndexFromNote(int note);

        void registerPadPress(int padIndex);
        void registerPadRelease(int padIndex);
        bool isPadRegisteredAsPressed(int padIndex) const;
        int getPadPressCount(int padIndex) const;
        bool isAnyPadRegisteredAsPressed() const;
        void clearPressedPadRegistry();

	private:
		Sampler* sampler = nullptr;
		std::string name;
		std::vector<NoteParameters*> noteParameters;
		std::vector<Pad*> pads;
		PgmSlider* slider = nullptr;
		int midiProgramChange = 0;

        /**
         * pressedPadRegistry tracks how many simultaneous "press" sources
         * (MIDI, keyboard, mouse, etc.) are currently active for each of the
         * 64 program pads (16 pads × 4 banks). A value > 0 means the pad is
         * considered pressed. Counts automatically decrease on release and
         * are reset when the program is deleted or reloaded.
         */
        std::array<int, 64> pressedPadRegistry {};

		void init();

	public:
		int getNumberOfSamples();
		void setName(std::string s);
		std::string getName();
		Pad* getPad(int i);
		std::vector<NoteParameters*> getNotesParameters();
        NoteParameters* getNoteParameters(int note);
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

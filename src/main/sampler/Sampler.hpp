#pragma once

#include <mpc/MpcSampler.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>

#include <set>
#include <memory>

namespace ctoot::mpc {
	class MpcSound;
	class MpcProgram;
	class MpcSoundPlayerChannel;
}

namespace mpc::sequencer {
	class NoteEvent;
}

namespace mpc::sampler {

	class Sampler final
		: public virtual ctoot::mpc::MpcSampler
	{

	public:
		std::weak_ptr<ctoot::mpc::MpcSound> getPreviewSound() override;
		std::weak_ptr<ctoot::mpc::MpcSound> getPlayXSound() override;
		std::weak_ptr<ctoot::mpc::MpcSound> getClickSound() override;
		std::weak_ptr<ctoot::mpc::MpcSound> getSound(int sampleNumber) override;
		std::weak_ptr<ctoot::mpc::MpcProgram> getProgram(int programNumber) override;

	private:
		int inputLevel = 0;
		int soundIndex = -1;
		int playX_ = 0;
		std::string previousScreenName = "";

	public:
		void setSoundIndex(int i);
		int getSoundIndex();
		std::weak_ptr<Sound> getSound();
		std::string getPreviousScreenName();
		void setPreviousScreenName(std::string s);


	private:
		std::vector<int> initMasterPadAssign;
		std::vector<int> masterPadAssign;
		std::vector<int> autoChromaticAssign;

		std::vector<std::shared_ptr<Sound>> sounds;
		std::vector<std::shared_ptr<Program>> programs = std::vector<std::shared_ptr<Program>>(24);
		int soundSortingType = 0;
		std::vector<std::string> padNames;
		std::vector<std::string> abcd = std::vector<std::string>{ "A", "B", "C", "D" };
		std::vector<float> clickSample;
		std::shared_ptr<Sound> clickSound;
		std::vector<std::string> sortNames = std::vector<std::string>{ "MEMORY", "NAME", "SIZE" };
		

	public:
		int getInputLevel();
		void setInputLevel(int i);

	public:
		void finishBasicVoice();
		void init();
		void playMetronome(mpc::sequencer::NoteEvent* event, int framePos);
		void playPreviewSample(int start, int end, int loopTo, int overlapMode);
		int getProgramCount();
		std::weak_ptr<Program> addProgram();
		std::weak_ptr<Program> addProgram(int i);
		void replaceProgram(std::weak_ptr<Program> p, int index);
		void deleteProgram(std::weak_ptr<Program> program);
		void deleteAllPrograms(bool init);
		void checkProgramReferences();
		std::vector<std::weak_ptr<Sound>> getSounds();
		std::weak_ptr<Sound> addSound();
		std::weak_ptr<Sound> addSound(int sampleRate);
		int getSoundCount();
		std::string getSoundName(int i);
		std::string getPadName(int i);
		std::vector<std::weak_ptr<Program>> getPrograms();
		std::vector<float>* getClickSample();
		void deleteSound(std::weak_ptr<Sound> sound);
		void setLoopEnabled(int sampleIndex, bool b);
		void trimSample(int sampleNumber, int start, int end);
		void deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end);
		void sort();

	private:
		void trimSample(std::weak_ptr<Sound>, int start, int end);

	private:
		static bool compareMemoryIndex(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
		static bool compareName(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
		static bool compareSize(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);

	public:
		void deleteSample(int sampleIndex);
		void deleteAllSamples();
		void process12Bit(std::vector<float>* fa);
		void process8Bit(std::vector<float>* fa);
		void resample(std::vector<float>*, int srcRate, std::vector<float>* dest, int destRate);
		std::weak_ptr<Sound> createZone(std::weak_ptr<Sound> source, int start, int end, int endMargin);
		void stopAllVoices();
		void stopAllVoices(int frameOffset);
		void playX();
		int getFreeSampleSpace();

	private:
		std::vector<std::weak_ptr<Sound>> getUsedSounds();
		int getLastInt(std::string s);
		std::string addOrIncreaseNumber2(std::string s);

	public:
		std::string addOrIncreaseNumber(std::string s);
		Pad* getLastPad(Program* program);
		NoteParameters* getLastNp(Program* program);
		int getUnusedSampleCount();
		void purge();
		static std::vector<float> mergeToStereo(std::vector<float> fa0, std::vector<float> fa1);
		void mergeToStereo(std::vector<float>* sourceLeft, std::vector<float>* sourceRight, std::vector<float>* dest);
		void setDrumBusProgramNumber(int busNumber, int programNumber);
		int getDrumBusProgramNumber(int busNumber);
		ctoot::mpc::MpcSoundPlayerChannel* getDrum(int i);

	public:
		int getUsedProgram(int startIndex, bool up);

	public:
		int checkExists(std::string soundName);
		int getNextSoundIndex(int j, bool up);
		void selectPreviousSound();
		void selectNextSound();
		std::vector<std::weak_ptr<ctoot::mpc::MpcStereoMixerChannel>> getDrumStereoMixerChannels(int i);
		std::vector<std::weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>> getDrumIndivFxMixerChannels(int i);
		std::weak_ptr<Sound> copySound(std::weak_ptr<Sound> source);

	public:
		std::vector<int>* getInitMasterPadAssign();
		std::vector<int>* getMasterPadAssign();
		void setMasterPadAssign(std::vector<int> v);
		std::vector<int>* getAutoChromaticAssign();
		void setPlayX(int i);
		int getPlayX();

	public:
		Sampler();

	};
}

#pragma once

#include <mpc/MpcSampler.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>

#include <memory>

namespace ctoot::mpc {
	class MpcSound;
	class MpcProgram;
	class MpcSoundPlayerChannel;
}

namespace mpc {
	
}

namespace mpc::sequencer {
	class NoteEvent;
}

using namespace ctoot::mpc;

using namespace std;

namespace mpc::sampler {

	class Sampler final
		: public virtual MpcSampler
	{

	public:
		weak_ptr<MpcSound> getPreviewSound() override;
		weak_ptr<MpcSound> getPlayXSound() override;
		weak_ptr<MpcSound> getClickSound() override;
		weak_ptr<MpcSound> getSound(int sampleNumber) override;
		weak_ptr<MpcProgram> getProgram(int programNumber) override;

	private:
		int inputLevel = 0;

	private:
		vector<int> initMasterPadAssign{};
		vector<int> masterPadAssign{};
		vector<int> autoChromaticAssign{};

		vector<shared_ptr<Sound>> sounds{};
		vector<shared_ptr<Program>> programs{};
		int soundSortingType{};
		vector<string> padNames{};
		vector<string> abcd{};
		vector<float> clickSample{};
		shared_ptr<Sound> clickSound{};
		vector<string> sortNames{};
		

	public:
		int getInputLevel();
		void setInputLevel(int i);

	public:
		void finishBasicVoice();
		void init();
		void playMetronome(mpc::sequencer::NoteEvent* event, int framePos);
		void playPreviewSample(int start, int end, int loopTo, int overlapMode);
		int getProgramCount();
		weak_ptr<Program> addProgram();
		weak_ptr<Program> addProgram(int i);
		void replaceProgram(weak_ptr<Program> p, int index);
		void deleteProgram(weak_ptr<Program> program);
		void deleteAllPrograms(bool init);
		void checkProgramReferences();
		vector<weak_ptr<Sound>> getSounds();
		weak_ptr<Sound> addSound();
		weak_ptr<Sound> addSound(int sampleRate);
		int getSoundCount();
		string getSoundName(int i);
		string getPadName(int i);
		vector<weak_ptr<Program>> getPrograms();
		vector<float>* getClickSample();
		void deleteSound(weak_ptr<Sound> sound);
		void setLoopEnabled(int sampleIndex, bool b);
		void trimSample(int sampleNumber, int start, int end);
		void deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end);
		void sort();

	private:
		void trimSample(weak_ptr<Sound>, int start, int end);

	private:
		static bool compareMemoryIndex(weak_ptr<Sound> a, weak_ptr<Sound> b);
		static bool compareName(weak_ptr<Sound> a, weak_ptr<Sound> b);
		static bool compareSize(weak_ptr<Sound> a, weak_ptr<Sound> b);

	public:
		void deleteSample(int sampleIndex);
		void deleteAllSamples();
		void process12Bit(vector<float>* fa);
		void process8Bit(vector<float>* fa);
		void resample(vector<float>*, int srcRate, vector<float>* dest, int destRate);
		weak_ptr<Sound> createZone(weak_ptr<Sound> source, int start, int end, int endMargin);
		void stopAllVoices();
		void stopAllVoices(int frameOffset);
		void playX(int playXMode, vector<int>* zone);
		int getFreeSampleSpace();

	private:
		int getLastInt(string s);
		string addOrIncreaseNumber2(string s);

	public:
		string addOrIncreaseNumber(string s);
		Pad* getLastPad(Program* program);
		NoteParameters* getLastNp(Program* program);
		int getUnusedSampleAmount();
		void purge();
		static vector<float> mergeToStereo(vector<float> fa0, vector<float> fa1);
		void mergeToStereo(vector<float>* sourceLeft, vector<float>* sourceRight, vector<float>* dest);
		void setDrumBusProgramNumber(int busNumber, int programNumber);
		int getDrumBusProgramNumber(int busNumber);
		ctoot::mpc::MpcSoundPlayerChannel* getDrum(int i);

	public:
		int getUsedProgram(int startIndex, bool up);

	private:
		void setSampleBackground();

	public:
		int checkExists(string soundName);
		int getNextSoundIndex(int j, bool up);
		void setSoundGuiPrevSound();
		void setSoundGuiNextSound();
		vector<weak_ptr<ctoot::mpc::MpcStereoMixerChannel>> getDrumStereoMixerChannels(int i);
		vector<weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>> getDrumIndivFxMixerChannels(int i);
		weak_ptr<Sound> copySound(weak_ptr<Sound> source);

	public:
		vector<int>* getInitMasterPadAssign();
		vector<int>* getMasterPadAssign();
		void setMasterPadAssign(vector<int> v);
		vector<int>* getAutoChromaticAssign();


	public:
		Sampler();
		~Sampler();

	};
}

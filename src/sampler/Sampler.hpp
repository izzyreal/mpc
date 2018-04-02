#pragma once

#include <observer/Observable.hpp>
#include <audio/server/AudioClient.hpp>
#include <audio/system/AudioDevice.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>

#include <audio/server/IOAudioProcess.hpp>
#include <audio/core/AudioBuffer.hpp>

#include <boost/circular_buffer.hpp>

#include <memory>

namespace mpc {

	class Mpc;

	namespace ctootextensions {
		class MpcSoundPlayerChannel;
	}

	namespace sequencer {
		class NoteEvent;
	}

	namespace sampler {

		class MonitorOutput;
		class StereoMixerChannel;
		class IndivFxMixerChannel;

		class Sampler
			: public virtual ctoot::audio::server::AudioClient
			, public virtual ctoot::audio::system::AudioDevice
		{

		private:
			int inputLevel = 0;
			float sampleRate;

		protected:
			std::vector<float>* monitorBufferL{ nullptr };
			std::vector<float>* monitorBufferR{ nullptr };

		private:
			std::vector<int> initMasterPadAssign{};
			std::vector<int> masterPadAssign{};
			std::vector<int> autoChromaticAssign{};

			std::unique_ptr<ctoot::audio::core::AudioBuffer> recordBuffer{};

			static const int VU_BUFFER_SIZE{ 100 };
			int vuCounter{};
			std::vector<float> vuBufferL{};
			std::vector<float> vuBufferR{};
			std::vector<float> recordBufferL{};
			std::vector<float> recordBufferR{};
			int vuSampleCounter{};
			bool recording{};
			int recordFrame{};
			ctoot::audio::server::IOAudioProcess* analogInput{ nullptr };
			ctoot::audio::server::IOAudioProcess* digitalInput{ nullptr };
			ctoot::audio::server::IOAudioProcess* input{ nullptr };
			ctoot::audio::server::IOAudioProcess* inputSwap{ nullptr };
			boost::circular_buffer<float> preRecBufferL{};
			boost::circular_buffer<float> preRecBufferR{};
			std::shared_ptr<MonitorOutput> monitorOutput{};
			std::vector<std::shared_ptr<Sound>> sounds{};
			std::vector<std::shared_ptr<Program>> programs{};
			int soundSortingType{};
			std::vector<std::string> padNames{};
			std::vector<std::string> abcd{};
			std::vector<float> clickSample{};
			std::shared_ptr<Sound> clickSound{};
			//std::shared_ptr<Sound> previewSound{};
			std::vector<std::string> sortNames{};
			bool armed{};
			float peakL{};
			float peakR{};
			float levelL{};
			float levelR{};
			mpc::Mpc* mpc{ nullptr };

		public:
			int getInputLevel();
			void setInputLevel(int i);

		public:
			void work(int nFrames) override;
			void setEnabled(bool b) override;
			void setSampleRate(int rate);

		public:
			void silenceRecordBuffer();
			void finishBasicVoice();
			void init();
			void playMetronome(mpc::sequencer::NoteEvent* event, int framePos);
			void playPreviewSample(int start, int end, int loopTo, int overlapMode);
			std::weak_ptr<Program> getProgram(int programNumber);
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
			std::weak_ptr<Sound> getSound(int sampleNumber);
			void deleteSound(std::weak_ptr<Sound> sound);
			std::weak_ptr<Sound> getPreviewSound();
			std::weak_ptr<Sound> getPlayXSound();
			void setLoopEnabled(int sampleIndex, bool b);
			void trimSample(int sampleNumber, int start, int end);
			void deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end);
			void sort();

		private:
			void trimSample(std::weak_ptr<Sound>, int start, int end);

		private:
			static bool memIndexCmp(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
			static bool nameCmp(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);
			static bool sizeCmp(std::weak_ptr<Sound> a, std::weak_ptr<Sound> b);

		public:
			void deleteSample(int sampleIndex);
			void deleteAllSamples();
			void process12Bit(std::vector<float>* fa);
			void process8Bit(std::vector<float>* fa);
			void resample(std::vector<float>*, int srcRate, std::vector<float>* dest, int destRate);
			std::weak_ptr<Sound> createZone(std::weak_ptr<Sound> source, int start, int end, int endMargin);
			void stopAllVoices();
			void stopAllVoices(int frameOffset);
			void playX(int playXMode, std::vector<int>* zone);
			int getFreeSampleSpace();

		private:
			int getLastInt(std::string s);
			std::string addOrIncreaseNumber2(std::string s);

		public:
			std::string addOrIncreaseNumber(std::string s);
			Pad* getLastPad(Program* program);
			NoteParameters* getLastNp(Program* program);
			int getUnusedSampleAmount();
			void purge();
			static std::vector<float> mergeToStereo(std::vector<float> fa0, std::vector<float> fa1);
			void mergeToStereo(std::vector<float>* sourceLeft, std::vector<float>* sourceRight, std::vector<float>* dest);
			void setDrumBusProgramNumber(int busNumber, int programNumber);
			int getDrumBusProgramNumber(int busNumber);
			mpc::ctootextensions::MpcSoundPlayerChannel* getDrum(int i);
			std::weak_ptr<Sound> getClickSound();

			void arm();
			void stopRecordingEarlier();

		public:
			int getUsedProgram(int startIndex, bool up);

		private:
			void record();
			void stopRecording();
			void stopRecordingBasic();


			/*
			* Implement ctoot::audio::system::AudioDevice
			*/
		public:
			std::string getName() override;
			std::vector<std::weak_ptr<ctoot::audio::system::AudioInput>> getAudioInputs() override;
			std::vector<std::weak_ptr<ctoot::audio::system::AudioOutput>> getAudioOutputs() override;
			void closeAudio() override;

		public:
			int getPeakL();
			int getPeakR();
			int getLevelL();
			int getLevelR();
			void resetPeak();
			bool isArmed();
			bool isRecording();
			void unArm();
			void cancelRecording();

		private:
			void setSampleBackground();

		public:
			int checkExists(std::string soundName);
			int getNextSoundIndex(int j, bool up);
			void setSoundGuiPrevSound();
			void setSoundGuiNextSound();
			std::vector<std::weak_ptr<mpc::sampler::StereoMixerChannel>> getDrumStereoMixerChannels(int i);
			std::vector<std::weak_ptr<mpc::sampler::IndivFxMixerChannel>> getDrumIndivFxMixerChannels(int i);
			std::weak_ptr<Sound> copySound(std::weak_ptr<Sound> source);
			void setActiveInput(ctoot::audio::server::IOAudioProcess* input);

		public:
			std::vector<int>* getInitMasterPadAssign();
			std::vector<int>* getMasterPadAssign();
			void setMasterPadAssign(std::vector<int> v);
			std::vector<int>* getAutoChromaticAssign();


		public:
			Sampler(mpc::Mpc* mpc);
			~Sampler();

		private:
			friend class MonitorOutput;

		};

	}
}

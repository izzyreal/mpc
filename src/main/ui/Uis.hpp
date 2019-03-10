#pragma once

#include <vector>
#include <string>

namespace mpc {
	class Mpc;
	namespace ui {

		class NameGui;

		namespace vmpc {
			class DirectToDiskRecorderGui;
			class AudioGui;
			class DeviceGui;
			class MidiGui;
		}

		namespace misc {
			class PunchGui;
			class SecondSeqGui;
			class TransGui;
		}

		namespace other {
			class OthersGui;
		}

		namespace disk {
			class DiskGui;
			namespace window {
				class DiskWindowGui;
				class DirectoryGui;
			}
		}

		namespace sequencer {
			class StepEditorGui;
			class BarCopyGui;
			class EditSequenceGui;
			class SongGui;
			class TrMoveGui;
			class SequencerGui;

			namespace window {
				class SequencerWindowGui;
				class EraseGui;
			}
		}

		namespace sampler {
			class SamplerGui;
			class SoundGui;
			class MixerGui;
			class MixerSetupGui;

			namespace window {
				class SamplerWindowGui;
				class EditSoundGui;
				class ZoomGui;
			}
		}

		namespace midisync {
			class MidiSyncGui;
		}

		class Uis
		{

		private:
			ui::NameGui* nameGui{ nullptr };

			ui::sequencer::SongGui* songGui{ nullptr };
			ui::sequencer::SequencerGui* sequencerGui{ nullptr };
			ui::sequencer::StepEditorGui* stepEditorGui{ nullptr };
			ui::sequencer::BarCopyGui* barCopyGui{ nullptr };
			ui::sequencer::EditSequenceGui* editSequenceGui{ nullptr };
			ui::sequencer::TrMoveGui* trMoveGui{ nullptr };

			ui::sequencer::window::EraseGui* eraseGui{ nullptr };
			ui::sequencer::window::SequencerWindowGui* sequencerWindowGui{ nullptr };

			ui::sampler::SoundGui* soundGui{ nullptr };
			ui::sampler::SamplerGui* samplerGui{ nullptr };
			ui::sampler::MixerGui* mixerGui{ nullptr };
			ui::sampler::MixerSetupGui* mixerSetupGui{ nullptr };

			ui::sampler::window::EditSoundGui* editSoundGui{ nullptr };
			ui::sampler::window::SamplerWindowGui* samplerWindowGui{ nullptr };
			ui::sampler::window::ZoomGui* zoomGui{ nullptr };

			ui::disk::DiskGui* diskGui{ nullptr };
			ui::disk::window::DirectoryGui* directoryGui{ nullptr };
			ui::disk::window::DiskWindowGui* diskWindowGui{ nullptr };

			ui::midisync::MidiSyncGui* midiSyncGui{ nullptr };

			ui::other::OthersGui* othersGui{ nullptr };

			ui::misc::PunchGui* punchGui{ nullptr };
			ui::misc::SecondSeqGui* secondSeqGui{ nullptr };
			ui::misc::TransGui* transGui{ nullptr };

			ui::vmpc::DeviceGui* deviceGui{ nullptr };
			ui::vmpc::MidiGui* midiGui{ nullptr };
			ui::vmpc::AudioGui* audioGui{ nullptr };
			ui::vmpc::DirectToDiskRecorderGui* d2dRecorderGui{ nullptr };

		public:
			ui::sequencer::SequencerGui* getSequencerGui();
			ui::sequencer::StepEditorGui* getStepEditorGui();
			ui::sampler::MixerGui* getMixerGui();
			ui::sequencer::EditSequenceGui* getEditSequenceGui();
			ui::sequencer::window::SequencerWindowGui* getSequencerWindowGui();
			ui::midisync::MidiSyncGui* getMidiSyncGui();
			ui::sequencer::BarCopyGui* getBarCopyGui();
			ui::sequencer::TrMoveGui* getTrMoveGui();
			ui::sampler::SoundGui* getSoundGui();
			ui::sampler::window::EditSoundGui* getEditSoundGui();
			ui::sampler::window::ZoomGui* getZoomGui();
			ui::disk::DiskGui* getDiskGui();
			ui::disk::window::DirectoryGui* getDirectoryGui();
			ui::disk::window::DiskWindowGui* getDiskWindowGui();
			ui::sampler::window::SamplerWindowGui* getSamplerWindowGui();
			ui::sampler::SamplerGui* getSamplerGui();
			ui::NameGui* getNameGui();
			ui::sequencer::SongGui* getSongGui();
			ui::sampler::MixerSetupGui* getMixerSetupGui();
			ui::sequencer::window::EraseGui* getEraseGui();

			ui::other::OthersGui* getOthersGui();
			ui::vmpc::DeviceGui* getDeviceGui();
			ui::vmpc::AudioGui* getAudioGui();
			ui::vmpc::MidiGui* getMidiGui();
			ui::vmpc::DirectToDiskRecorderGui* getD2DRecorderGui();

			ui::misc::TransGui* getTransGui();
			ui::misc::SecondSeqGui* getSecondSeqGui();
			ui::misc::PunchGui* getPunchGui();

		private:
			std::vector<std::string> someNoteNames{ "C.", "C#", "D.", "D#", "E.", "F.", "F#", "G.", "G#", "A.", "A#", "B." };

		public:
			static std::vector<std::string> noteNames;

		public:
			Uis(mpc::Mpc* mpc);
			~Uis();

		};
	}
}

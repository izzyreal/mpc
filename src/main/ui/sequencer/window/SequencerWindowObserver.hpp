#pragma once
#include <observer/Observer.hpp>

#include <sequencer/TimeSignature.hpp>

#include <memory>
#include <vector>

namespace ctoot::mpc
{
	class MpcSoundPlayerChannel;
}

namespace mpc::sequencer
{
	class Sequencer;
	class Sequence;
	class Track;
}

namespace mpc::sampler
{
	class Sampler;
	class Program;
}

namespace mpc::lcdgui
{
	class FunctionKeys;
	class Field;
	class Label;
}

namespace mpc::ui
{
	class NameGui;
}

namespace mpc::ui::sampler
{
	class SamplerGui;
}

namespace mpc::ui::sequencer::window
{
	class SequencerWindowGui;
}

namespace mpc::ui::sequencer::window
{

	class SequencerWindowObserver
		: public moduru::observer::Observer
	{

	private:
		std::string csn{ "" };
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		std::weak_ptr<mpc::sampler::Program> program;
		int trackNum = 0;
		int seqNum = 0;
		std::weak_ptr<mpc::sequencer::Sequence> sequence;
		std::weak_ptr<mpc::sequencer::Track> track;

		SequencerWindowGui* swGui = nullptr;
		mpc::ui::NameGui* nameGui = nullptr;
		std::weak_ptr<mpc::lcdgui::Field> sqField;
		std::weak_ptr<mpc::lcdgui::Field> trField;
		std::weak_ptr<mpc::lcdgui::Field> trackNameFirstLetterField;
		std::weak_ptr<mpc::lcdgui::Field> defaultTrackNameFirstLetterField;
		std::weak_ptr<mpc::lcdgui::Label> defaultTrackNameRestLabel;
		std::weak_ptr<mpc::lcdgui::Label> trackNameRestLabel;
		std::weak_ptr<mpc::lcdgui::Field> sq0Field;
		std::weak_ptr<mpc::lcdgui::Field> sq1Field;
		std::weak_ptr<mpc::lcdgui::Field> tr0Field;
		std::weak_ptr<mpc::lcdgui::Field> tr1Field;
		ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel;
		mpc::lcdgui::FunctionKeys* fb;
		std::weak_ptr<mpc::lcdgui::Field> inThisTrackField;
		mpc::ui::sampler::SamplerGui* samplerGui = nullptr;

	public:
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	private:
		void displayTrackNumber();
		void displaySequenceNumberName();
		void displaySequenceNumberNames();
		void displayTrackNumberNames();

	public:
		SequencerWindowObserver();
		~SequencerWindowObserver();

	};
}

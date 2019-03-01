#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>

namespace mpc {
	class Mpc;
	namespace sequencer {
		class Sequencer;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {
		namespace sampler {
			class SamplerGui;
		}

		namespace sequencer {

			class NextSeqPadObserver
				: public moduru::observer::Observer
			{

			private:
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				mpc::ui::sampler::SamplerGui* samplerGui{};
				std::weak_ptr<mpc::lcdgui::Field> sqField{};
				std::weak_ptr<mpc::lcdgui::Field> now0Field{};
				std::weak_ptr<mpc::lcdgui::Field> now1Field{};
				std::weak_ptr<mpc::lcdgui::Field> now2Field{};
				std::vector<std::weak_ptr<mpc::lcdgui::Label>> seqs{};
				std::weak_ptr<mpc::lcdgui::Label> seq1{};
				std::weak_ptr<mpc::lcdgui::Label> seq2{};
				std::weak_ptr<mpc::lcdgui::Label> seq3{};
				std::weak_ptr<mpc::lcdgui::Label> seq4{};
				std::weak_ptr<mpc::lcdgui::Label> seq5{};
				std::weak_ptr<mpc::lcdgui::Label> seq6{};
				std::weak_ptr<mpc::lcdgui::Label> seq7{};
				std::weak_ptr<mpc::lcdgui::Label> seq8{};
				std::weak_ptr<mpc::lcdgui::Label> seq9{};
				std::weak_ptr<mpc::lcdgui::Label> seq10{};
				std::weak_ptr<mpc::lcdgui::Label> seq11{};
				std::weak_ptr<mpc::lcdgui::Label> seq12{};
				std::weak_ptr<mpc::lcdgui::Label> seq13{};
				std::weak_ptr<mpc::lcdgui::Label> seq14{};
				std::weak_ptr<mpc::lcdgui::Label> seq15{};
				std::weak_ptr<mpc::lcdgui::Label> seq16{};
				std::weak_ptr<mpc::lcdgui::Label> seqNumbers{};
				std::weak_ptr<mpc::lcdgui::Label> bank{};
				std::weak_ptr<mpc::lcdgui::Label> nextSqLabel{};

			private:
				void displayNextSq();
				int bankoffset();
				void displayBank();
				void displaySeqNumbers();
				void displaySq();
				void setOpaque(int i);
				void displaySeq(int i);
				void setSeqColor(int i);

			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

			private:
				void displayNow0();
				void displayNow1();
				void displayNow2();
				void refreshSeqs();

			public:
				NextSeqPadObserver(mpc::Mpc* mpc);
				~NextSeqPadObserver();

			};

		}
	}
}

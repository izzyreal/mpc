#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::disk
{
	class MpcFile;
}

namespace mpc::sequencer
{
	class Sequence;
}

namespace mpc::file::all
{
	class Sequence;
    class AllParser;
}

namespace mpc::disk
{
	class AllLoader
	{

	private:
		mpc::Mpc& mpc;
		std::vector<mpc::file::all::Sequence*> allSequences;
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>> mpcSequences;

		void convertSequences(const bool indiv);

	public:
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>>& getSequences();
		AllLoader(mpc::Mpc&, mpc::disk::MpcFile*, bool sequencesOnly);
        AllLoader(mpc::Mpc&, mpc::file::all::AllParser&, bool sequencesOnly);

		static std::vector<std::shared_ptr<mpc::sequencer::Sequence>> loadOnlySequencesFromFile(mpc::Mpc&, mpc::disk::MpcFile*);
	};
}

#pragma once

#include <memory>
#include <vector>

#include <file/all/AllParser.hpp>

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
}

namespace mpc::disk
{
	class AllLoader
	{

	private:
		mpc::Mpc& mpc;
		mpc::file::all::AllParser allParser;
		std::vector<mpc::file::all::Sequence*> allSequences;
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>> mpcSequences;

		void convertSequences(const bool indiv);

	public:
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>>& getSequences();
		AllLoader(mpc::Mpc& mpc, mpc::disk::MpcFile* file, bool sequencesOnly);

	};
}

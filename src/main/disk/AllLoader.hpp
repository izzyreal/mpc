#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::disk
{
	class MpcFile;
}

namespace mpc::file::all {
	class AllParser;
}

namespace mpc::sequencer
{
	class Sequence;
}

namespace mpc::disk {
	class AllLoader
	{
	public:
		static std::vector<std::shared_ptr<mpc::sequencer::Sequence>> loadOnlySequencesFromFile(mpc::Mpc&, mpc::disk::MpcFile*);
		static void loadEverythingFromFile(mpc::Mpc&, mpc::disk::MpcFile*);
		static void loadEverythingFromAllParser(mpc::Mpc&, mpc::file::all::AllParser&);
	};
}

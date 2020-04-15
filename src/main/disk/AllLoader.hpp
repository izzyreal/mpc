#pragma once

#include <memory>
#include <vector>

namespace mpc {
	class Mpc;
}

namespace mpc::ui {
	class Uis;
}

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::sequencer {
	class Sequence;
}

namespace mpc::file::all {
	class AllParser;
	class Sequence;
}

namespace mpc::disk {

	class AllLoader {

	private:
		mpc::file::all::AllParser* allParser{};
		std::vector<mpc::file::all::Sequence*> sequencesALL{};
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>> mpcSequences{};
		mpc::Mpc* mpc{};

	private:
		void convertSequences(const bool indiv);

	public:
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>>* getSequences();

		AllLoader(mpc::disk::MpcFile* file);
		AllLoader(mpc::Mpc* mpc, mpc::disk::MpcFile* file);

	};
}

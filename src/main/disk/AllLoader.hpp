#pragma once

#include <memory>
#include <vector>

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
		mpc::file::all::AllParser* allParser = nullptr;
		std::vector<mpc::file::all::Sequence*> allSequences;
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>> mpcSequences;
		

	private:
		void convertSequences(const bool indiv);

	public:
		std::vector<std::shared_ptr<mpc::sequencer::Sequence>>* getSequences();

	public:
		AllLoader(mpc::disk::MpcFile* file, bool sequencesOnly);
		~AllLoader();
	};
}
